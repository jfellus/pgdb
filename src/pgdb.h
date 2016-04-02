/*
 * pgdb.h
 *
 *  Created on: 25 mai 2015
 *      Author: jfellus
 */

#ifndef PGDB_H_
#define PGDB_H_

#include "Plug.h"

#define _AUTORUN static void _do_autorun__##__LINE__(); static bool _bdo_autorun__##__LINE__(); static bool _autorun__##__LINE__ = _bdo_autorun__##__LINE__(); bool _bdo_autorun__##__LINE__() {_autorun__##__LINE__=true;  _do_autorun__##__LINE__(); return true;} void _do_autorun__##__LINE__ ()

class Peer;
void pgdb_create_server(long port);


/////////////////////
// AVAILABLE PLUGS //
/////////////////////

Plug* pgdb_register_plug(Plug* plug);
Plug* pgdb_get_plug(const char* variable);
void pgdb_list_plugs();
void pgdb_list_plugs(Peer* p);

//////////////
// PLUGGING //
//////////////

void pgdb_plug(Peer* peer, char* _what);
void pgdb_unplug(Peer* peer, char* _what);
void pgdb_plug_set(Peer* peer, char* _what);
void pgdb_plug_get(Peer* peer, const char* variable);

void pgdb_plug(const char* variable, Peer* peer, unsigned short channel);
void pgdb_unplug(const char* variable, Peer* peer);
void pgdb_unplug(Peer* peer, unsigned short channel);


///////////////////////
// PLUGS DECLARATION //
///////////////////////


Plug* pgdb_declare_plug_member(Plug* plug);
ModulePlugs* pgdb_init_module(void* script, void* module, const char* modulename);
void pgdb_init_script(void* parent, void* the_script, const char* scriptname);



//////////////////////
// PAUSE / STEP ... //
//////////////////////

void pgdb_start();
void pgdb_pause();
void pgdb_play();
void pgdb_step();

////////////////////
// MODULES MACROS //
////////////////////

#ifdef DEBUG

#define CONCAT2(x,y) x ## y
#define CONCAT(x, y) CONCAT2(x, y)

#undef ___dbs
#define ___dbs(plug) plug->send();

#undef PARAM
#define PARAM(type, var, callback) \
		template <typename T> static void CONCAT(__param_set_,__LINE__)(T* p) { p->CONCAT(__param_setter_,__LINE__)(); } \
		void CONCAT(__param_setter_,__LINE__)() { callback } \
		Plug* CONCAT(__dbg_plug_,__LINE__) = pgdb_declare_plug_member(new Plug(this, #var, #type, new PlugDataPtr<type>(& var, create_callback_fn(this,&CONCAT(__param_set_,__LINE__)))));

#undef OUTPUT
#define OUTPUT(type, var) \
		Plug* CONCAT(__dbg_plug_,__LINE__) = pgdb_declare_plug_member(new Plug(this, #var, #type, new PlugDataPtrReadOnly<type>(& var))); \
		inline operator type &() {return var;}

#else
#ifndef ___dbs
#define ___dbs(plug)
#endif
#ifndef PARAM
#define PARAM(type,var,callback)
#endif
#endif


#endif /* PGDB_H_ */
