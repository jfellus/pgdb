/*
 * plug.cpp
 *
 *  Created on: 25 mai 2015
 *      Author: jfellus
 */


#include "Plug.h"
#include "utils/Socket.h"


void Plug::plug(Peer* peer, unsigned short channel) {
	peer->plug(this, channel);
	plugs.add(new PlugInfo(peer, channel));
}

void Plug::unplug(Peer* peer, unsigned short channel) {
	for(uint i=0; i<plugs.size(); i++) if(plugs[i]->peer==peer && plugs[i]->channel==channel) {
		delete plugs[i];
		plugs.remove(plugs[i--]);
	}
}

void Plug::unplug_all(Peer* peer) {
	for(uint i=0; i<plugs.size(); i++) if(plugs[i]->peer==peer) {
		delete plugs[i];
		plugs.remove(plugs[i--]);
	}
}


///////////////
// DATA FLOW //
///////////////

void Plug::send(bool force) {
	if(!force && is_only_on_request()) return;
	for(uint i=0; i<plugs.size(); i++) {
		if(plugs[i]->bFirst) send_infos(plugs[i]);
		plugs[i]->peer->send(plugs[i]->channel, (unsigned char*)get_value(), get_value_size());
	}
}

void Plug::send_infos(PlugInfo* p) {
	std::string s = get_info();
	size_t n = s.length()+sizeof(uint);
	char* infos = new char[n];
	memcpy(infos, &p->channel, sizeof(uint));
	memcpy(infos+sizeof(uint), s.c_str(), s.length());
	p->peer->send((uint)-2, (unsigned char*)infos, n);
	p->bFirst = false;
}
