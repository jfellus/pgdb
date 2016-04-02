/*
 * Socket.h
 *
 *  Created on: 25 mai 2015
 *      Author: jfellus
 */

#ifndef PGDB_SOCKET_H_
#define PGDB_SOCKET_H_

#include "utils.h"
#include <libwebsockets.h>
#include "../pgdb.h"



class Peer {
public:
	void* socket;
	pthread_mutex_t mut;

	array<Plug*> plugs;

private:
	unsigned char* msg;

public:
	Peer(void* socket) {msg = 0; this->socket = socket; pthread_mutex_init(&mut, 0);}
	~Peer();

	/////////
	// COM //
	/////////

	void recv(unsigned char* data, size_t size); // Callback
	void send(uint channel, unsigned char* data, size_t size);

	///////////////
	// ACCESSORS //
	///////////////

	unsigned short get_channel(const char* variable) {
		for(uint i=0; i<plugs.size(); i++) if(plugs[i]->variable==variable) return i;
		return (unsigned short)-1;
	}

	const char* get_variable(unsigned short channel) {
		if(channel>=plugs.size()) return NULL;
		return plugs[channel]->variable.c_str();
	}

	Plug* get_plug(const char* variable) {
		for(uint i=0; i<plugs.size(); i++) if(plugs[i]->variable==variable) return plugs[i];
		return NULL;
	}

	Plug* get_plug(unsigned short channel) {
		if(channel>=plugs.size()) return NULL;
		return plugs[channel];
	}


	/////////////
	// METHODS //
	/////////////

	void plug(Plug* plug, unsigned short channel) {
		while(channel>=plugs.size()) plugs.add(NULL);
		plugs[channel] = plug;
	}

	void unplug(const char* variable) {	unplug(get_channel(variable));	}

	void unplug(unsigned short channel) {
		for(uint i=0; i<plugs.size(); i++) plugs[i]->unplug(this, channel);
	}


	void plug_set(const char* variable, const char* value) {
		get_plug(variable)->set_value(value);
	}

	void plug_get(const char* variable) {
		get_plug(variable)->send(true);
	}

};


#endif /* SOCKET_H_ */
