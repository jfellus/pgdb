/*
 * Plug.h
 *
 *  Created on: 25 mai 2015
 *      Author: jfellus
 */

#ifndef PLUG_H_
#define PLUG_H_

#include "utils/utils.h"
#include "plugdata/PlugData.h"

class Peer;


class PlugInfo {
public:
	Peer* peer;
	unsigned short channel;
	bool bFirst;
	PlugInfo(Peer* peer, unsigned short channel): peer(peer), channel(channel) {bFirst = true;}
};


class Plug {
public:
	std::string variable;
	std::string type;
	array<PlugInfo*> plugs;
	PlugData* plugData;
	void* module;

	Plug(void* module, const char* variable, const char* type, PlugData* plugData) {
		this->module = module;
		this->variable = variable;
		this->type = type;
		this->plugData = plugData;
	}

	void set_context(const std::string& context) {
		std::string tmp = variable;
		variable = TOSTRING(context << "." << variable);
	}

	//////////////
	// PLUGGING //
	//////////////

	void plug(Peer* peer, unsigned short channel);
	void unplug(Peer* peer, unsigned short channel);

	void unplug_all(Peer* peer);

	void send(bool force = false);
	void send_infos(PlugInfo* p);

	//////////
	// DATA //
	//////////

	std::string get_info() { return plugData->get_info(); }
	void set_value(const char* value) { plugData->set(value); }
	void* get_value() { return plugData->get(); }
	size_t get_value_size() { return plugData->get_size(); }
	bool is_only_on_request() {return plugData->is_only_on_request(); }

};



//////////////////
// MODULE PLUGS //
//////////////////

class ModulePlugs {
public:
	array<Plug*> plugs;

	ModulePlugs() {}

	void add(Plug* plug) { plugs.add(plug); }

	void set_context(const std::string& context) {
		for(uint i=0; i<plugs.size(); i++) plugs[i]->set_context(context);
	}

	void send() {for(uint i=0; i<plugs.size(); i++) plugs[i]->send();}

};


#endif /* PLUG_H_ */
