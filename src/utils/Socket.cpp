/*
 * Socket.cpp
 *
 *  Created on: 25 mai 2015
 *      Author: jfellus
 */


#include "Socket.h"


//////////
// PEER //
//////////

Peer::~Peer() {
	for(uint i=0; i<plugs.size(); i++) plugs[i]->unplug(this, i);
}

void Peer::recv(unsigned char* data, size_t size) {
	if(data[0]=='P') pgdb_plug(this, (char*)&data[1]);
	else if(data[0]=='U') pgdb_unplug(this, (char*)&data[1]);
	else if(data[0]=='S') pgdb_plug_set(this, (char*)&data[1]);
	else if(data[0]=='G') pgdb_plug_get(this, (char*)&data[1]);
	else if(data[0]=='Y') pgdb_play();
	else if(data[0]=='L') pgdb_list_plugs(this);
}

void Peer::send(uint channel, unsigned char* data, size_t size) {
	pthread_mutex_lock(&mut);
	size_t msg_size = sizeof(channel) + size;
	size_t total_size = msg_size + LWS_SEND_BUFFER_PRE_PADDING+LWS_SEND_BUFFER_POST_PADDING;
	if(msg) delete msg;
	msg = new unsigned char[total_size];
	unsigned char* _msg = msg + LWS_SEND_BUFFER_PRE_PADDING;
	memcpy(_msg, &channel, sizeof(channel)); _msg += sizeof(channel);
	memcpy(_msg, data, size);
	libwebsocket_write((struct libwebsocket*)socket,msg+LWS_SEND_BUFFER_PRE_PADDING, msg_size, LWS_WRITE_BINARY);
	pthread_mutex_unlock(&mut);
}



////////////
// SERVER //
////////////

#include <map>
static std::map<struct libwebsocket *, Peer*> peers;

static int callback_http(struct libwebsocket_context* _this, struct libwebsocket *wsi, enum libwebsocket_callback_reasons reason, void *user, void *in, size_t len) {return 0;}
static int callback_pgcc_debug(struct libwebsocket_context * _this, struct libwebsocket *wsi, enum libwebsocket_callback_reasons reason, void *user, void *in, size_t len) {
	std::map<struct libwebsocket *, Peer*>::iterator i;
	switch (reason) {
	case LWS_CALLBACK_ESTABLISHED:
		peers[wsi] = new Peer(wsi);
		//printf("connection established\n");
		break;
	case LWS_CALLBACK_CLOSED:
		i = peers.find(wsi);
		peers.erase(peers.find(wsi));
		delete (*i).second;
		//printf("connection lost\n");
		break;
	case LWS_CALLBACK_RECEIVE:
		peers[wsi]->recv((unsigned char*)in, len);
		break;
	default:
		break;
	}
	return 0;
}

static struct libwebsocket_protocols protocols[] = {
		{"http-only",callback_http,0},
		{"pgcc_debug", callback_pgcc_debug,0},
		{NULL, NULL, 0}
};

pthread_t th;
int PORT = 0;
static void* socket_main(void*) {
	struct lws_context_creation_info info;
	memset(&info, 0, sizeof(info));
	info.port = PORT;
	info.protocols = protocols;
	info.uid = -1; info.gid = -1;
	lws_set_log_level(0,NULL);
	struct libwebsocket_context* context = libwebsocket_create_context(&info);
	if (context == NULL) {
		fprintf(stderr, "libwebsocket init failed\n");
		exit(1);
	}
	while (1) libwebsocket_service(context, 50);
	libwebsocket_context_destroy(context);
	return 0;
}

void pgdb_create_server(long port) {
	PORT = port;
	pthread_create(&th, NULL, socket_main, NULL);
}
