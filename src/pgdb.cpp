/*
 * pgdb.cpp
 *
 *  Created on: 25 mai 2015
 *      Author: jfellus
 */

#include "pgdb.h"
#include "utils/Socket.h"
#include <map>
#include <semaphore.h>


////////////////////
// PLUGS REGISTRY //
////////////////////

static std::map<std::string, Plug*> plugs;

Plug* pgdb_register_plug(Plug* p) {
	plugs[p->variable] = p;
	return p;
}

Plug* pgdb_get_plug(const char* variable) {
	if(plugs.count(variable)==0) DBG("ERROR : No such variable : " << variable);
	return plugs[variable];
}


///////////
// PLUGS //
///////////

static std::map<void*,ModulePlugs> modules_plugs;
static std::map<void*, std::string> context;

Plug* pgdb_declare_plug_member(Plug* plug) {
	modules_plugs[plug->module].add(plug);
	return plug;
}


ModulePlugs* pgdb_init_module(void* script, void* module, const char* modulename) {
	ModulePlugs* mp = &modules_plugs[module];
	mp->set_context(TOSTRING(context[script] << "." << modulename));
	for(uint i=0; i<mp->plugs.size(); i++) pgdb_register_plug(mp->plugs[i]);
	return mp;
}

void pgdb_init_script(void* parent, void* the_script, const char* scriptname) {
	if(parent) context[the_script] = TOSTRING(context[parent] << "." << scriptname);
	else context[the_script] = scriptname;
}

void pgdb_list_plugs() {
	DBG("PLUGS ("<<plugs.size());
	DBG("----------");
	for(std::map<std::string, Plug*>::iterator i = plugs.begin(); i!=plugs.end(); i++) {
		DBG((*i).first);
	}
	DBG("----------");
}

void pgdb_list_plugs(Peer* p) {
	std::ostringstream oss;
	for(std::map<std::string, Plug*>::iterator i = plugs.begin(); i!=plugs.end(); i++) {
		oss << ((*i).first) << "," << ((*i).second)->type << "," << ((*i).second)->plugData->bReadOnly << "\n";
	}
	std::string s = oss.str();
	p->send((uint)-1, (unsigned char*)s.c_str(), s.size());
}

//////////////
// PLUGGING //
//////////////

void pgdb_plug(const char* variable, Peer* peer, unsigned short channel) {
	pgdb_get_plug(variable)->plug(peer, channel);
}

void pgdb_unplug(const char* variable, Peer* peer) {
	peer->unplug(variable);
}

void pgdb_unplug(Peer* peer, unsigned short channel) {
	peer->unplug(channel);
}

void pgdb_plug_set(Peer* peer, const char* variable, const char* value) {
	peer->plug_set(variable, value);
}

void pgdb_plug_get(Peer* peer, const char* variable) {
	peer->plug_get(variable);
}




/////////////
// PARSING //
/////////////


void pgdb_plug(Peer* peer, char* _what) {
	char* variable = _what;
	char* channel = strchr(variable, ',');
	*(channel++)=0;
	pgdb_plug(variable, peer, atoi(channel));
}

void pgdb_unplug(Peer* peer, char* _what) {
	pgdb_unplug(peer, atoi(_what));
}

void pgdb_plug_set(Peer* peer, char* _what) {
	char* variable = _what;
	char* value = strchr(variable, ',');
	*(value++)=0;
	pgdb_plug_set(peer, variable, value);
}


//////////////////////////////
// PAUSE / PLAY / STEP .... //
//////////////////////////////

void pgdb_start() {
	pgdb_create_server(10001);
	//pgdb_list_plugs();
	//pgdb_pause();
}

static bool bStep;
static sem_t sem_pause;
_AUTORUN { 	sem_init(&sem_pause, 0, 0); }

void pgdb_pause() {	sem_wait(&sem_pause);}

void pgdb_play() {
	bStep = false;
	sem_post(&sem_pause);
}

void pgdb_step() {
	bStep = true;
	sem_post(&sem_pause);
}
