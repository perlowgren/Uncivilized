
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include "main.h"
#include "game.h"



/*
	CMD_UNSPECIFIED,
	CMD_TEXT_MESSAGE,
	CMD_WELCOME,
	CMD_REQUEST_CLIENTS,
	CMD_SEND_CLIENTS,
	CMD_KILL_CLIENT,
	CMD_ACTIVATE_CLIENT,
	CMD_REQUEST_GAME,
	CMD_SEND_GAME,
	CMD_START_GAME,
	CMD_STOP_GAME,
	CMD_SHOW_FOW,
	CMD_BEGIN_TURN,
	CMD_END_TURN,
	CMD_CITY_PRODUCTION,
	CMD_CITY_RESOURCE,
	CMD_MOVE_UNIT,
	CMD_DISBAND_UNIT,
*/
const char *Game::cmd_names[] = {
	"CMD_UNSPECIFIED",
	"CMD_TEXT_MESSAGE",
	"CMD_WELCOME",
	"CMD_REQUEST_CLIENTS",
	"CMD_SEND_CLIENTS",
	"CMD_KILL_CLIENT",
	"CMD_ACTIVATE_CLIENT",
	"CMD_REQUEST_GAME",
	"CMD_SEND_GAME",
	"CMD_START_GAME",
	"CMD_STOP_GAME",
	"CMD_SHOW_FOW",
	"CMD_BEGIN_TURN",
	"CMD_END_TURN",
	"CMD_PLAYER_CLAIM_CITY",
	"CMD_CITY_PRODUCTION",
	"CMD_CITY_RESOURCE",
	"CMD_MOVE_UNITS",
	"CMD_DISBAND_UNIT",
	"CMD_DISBAND_UNITS",
};



void Game::sendServerCommand(aConnection c,int cmd,int v) {
	uint8_t d[SOCKET_HEADER+4],*p = d;
	pack_header(&p,cmd);
	pack_int32(&p,(int32_t)v);
debug_output("Game::sendServerCommand(cmd=%s,len=%d)\n",cmd_names[cmd],(int)sizeof(d));
	server.send(c,d,sizeof(d));
}

void Game::sendClientCommand(int cmd,int v) {
	uint8_t d[SOCKET_HEADER+4],*p = d;
	pack_header(&p,cmd);
	pack_int32(&p,(int32_t)v);
debug_output("Game::sendClientCommand(cmd=%s,len=%d)\n",cmd_names[cmd],(int)sizeof(d));
	client.send(d,sizeof(d));
}

void Game::sendServerData(aConnection c,int cmd,const uint8_t *data,int len) {
	uint8_t d[SOCKET_HEADER+len],*p = d;
	pack_header(&p,cmd);
	memcpy(p,data,len);
debug_output("Game::sendServerData(cmd=%s,len=%d)\n",cmd_names[cmd],(int)sizeof(d));
	server.send(c,d,sizeof(d));
}

void Game::sendClientData(int cmd,const uint8_t *data,int len) {
	uint8_t d[SOCKET_HEADER+len],*p = d;
	pack_header(&p,cmd);
	memcpy(p,data,len);
debug_output("Game::sendClientData(cmd=%s,len=%d)\n",cmd_names[cmd],(int)sizeof(d));
	client.send(d,sizeof(d));
}

void Game::sendServerMessage(aConnection c,int type,const char *format, ...) {
debug_output("Game::sendServerMessage(1,c=%p,type=%d)\n",c,type);
	uint8_t d[SOCKET_HEADER+5+257],*p = d;
//debug_output("Game::sendServerMessage(2,p=%p)\n",p);
	pack_header(&p,CMD_TEXT_MESSAGE);
//debug_output("Game::sendServerMessage(3,p=%p)\n",p);
	pack_uint32(&p,(uint32_t)0);
//debug_output("Game::sendServerMessage(4,p=%p)\n",p);
	pack_uint8(&p,(uint8_t)type);
//debug_output("Game::sendServerMessage(5,p=%p)\n",p);
	va_list args;
   va_start(args,format);
	vsnprintf((char *)p,256,format,args);
   va_end(args);
debug_output("Game::sendServerMessage(6,d=");
aSocket::print_packet(d,SOCKET_HEADER+6+strlen((const char *)p));
debug_output(")\n");
	if(*p) server.send(c,d,SOCKET_HEADER+6+strlen((const char *)p));
//debug_output("Game::sendServerMessage(1)\n");
}

void Game::sendClientMessage(int type,const char *format, ...) {
	uint8_t d[SOCKET_HEADER+5+257],*p = d;
	pack_header(&p,CMD_TEXT_MESSAGE);
	pack_uint32(&p,(uint32_t)client.getID());
	pack_uint8(&p,(uint8_t)type);
	va_list args;
   va_start(args,format);
	vsnprintf((char *)p,256,format,args);
   va_end(args);
debug_output("Game::sendClientMessage(%s)\n",(const char *)p);
	if(*p) client.send(d,SOCKET_HEADER+6+strlen((const char *)p));
}

void Game::sendGame() {
debug_output("Game::sendGame(clients=%d)\n",(int)server.clients());
	uint8_t *d;
	int l;
	packGame(CMD_SEND_GAME,&d,l);
	aConnection c;
	for(aHashtable::iterator iter=server.getClients(); (c=(aConnection)iter.next());) {
debug_output("Game::sendGame(client=%d,%s)\n",c->getID(),c->getNick());
		if(c->getID()!=client.getID()) server.send(c,d,l);
		sendServerCommand(c,CMD_START_GAME,1);
	}
	free(d);
}

void Game::sendGame(aConnection c) {
debug_output("Game::sendGame()\n");
	if(!isGameRunning()) return;
	uint8_t *d;
	int l;
	packGame(CMD_SEND_GAME,&d,l);
debug_output("Game::sendGame(len=%d)\n",l);
	server.send(c,d,l);
	free(d);
	sendServerCommand(c,CMD_START_GAME,1);
}

void Game::sendClient(aConnection c) {
debug_output("Game::sendClient()\n");
	aHashtable::iterator iter;
	Player *pl = (Player *)c->getData();
	uint8_t data[SOCKET_HEADER+26],*p = data;
	pack_header(&p,CMD_SEND_CLIENTS);
	pack_uint32(&p,0);
	pack_uint32(&p,c->getID());
	memcpy(p,c->getNick(),13);
	p[12] = '\0';
	p += 13;
	if(pl) {
		pack_uint32(&p,pl->status);
		pack_uint8(&p,pl->alliance);
	}
	server.send(data,sizeof(data));
}

void Game::sendClients(aConnection c) {
debug_output("Game::sendClients(c=%p)\n",c);
	aHashtable::iterator iter;
	aConnection c1;
	Player *pl;
debug_output("Game::sendClients(1)\n");
	uint8_t data[SOCKET_HEADER+4+22*server.clients()],*p = data;
	pack_header(&p,CMD_SEND_CLIENTS);
	pack_uint32(&p,(uint32_t)server.clients());
debug_output("Game::sendClients(3,n=%d)\n",(int)server.clients());
	for(iter=server.getClients(); (c1=(aConnection)iter.next());) {
debug_output("Game::sendClients(3.1, client(%d,'%s'))\n",c1->getID(),c1->getNick());
		pack_uint32(&p,c1->getID());
//debug_output("Game::sendClients(3.2)\n");
		memcpy(p,c1->getNick(),13);
		p[12] = '\0';
//debug_output("Game::sendClients(3.3)\n");
		p += 13;
		if((pl=(Player *)c1->getData())) {
			pack_uint32(&p,pl->status);
			pack_uint8(&p,pl->alliance);
		} else {
			pack_uint32(&p,0);
			pack_uint8(&p,0);
		}
//debug_output("Game::sendClients(3.4)\n");
//debug_output("Game::sendClients(3.5)\n");
	}
debug_output("Game::sendClients(4)\n");
	server.send(c,data,sizeof(data));
}

int Game::receiveClients(uint8_t *data) {
	int ret;
	aConnection c;
	Player *pl;
	uint32_t i,n;
	data += SOCKET_HEADER;
	unpack_uint32(&data,n);
debug_output("\nReceive Clients [%d]:\n",n);
	ret = n;
	if(n==0) n = 1;
	for(i=0; i<n; i++) {
		pl = new Player();
		unpack_uint32(&data,pl->id);
		memcpy(pl->nick,(const char *)data,13);
		data += 13;
		unpack_uint32(&data,pl->status);
		unpack_uint8(&data,pl->alliance);
debug_output("[id:%d] Nick: \"%s\", Status: %d\n",pl->id,pl->nick,pl->status);
		clients.put(pl->id,pl);
		players.put(pl->nick,pl);
		if(pl->id==client.getID()) local.player = pl;
		if(server.isRunning()) {
			c = server.getClient(pl->id);
			c->setData(pl);
		}
	}
debug_output("\n");
	if(setup.dialog) setup.dialog->updatePlayers();
	return ret;
}

/*
	SM_ERR_RESOLVE_HOST			= 1,
	SM_ERR_OPEN_SOCKET,
	SM_ERR_CONNECT,
	SM_ERR_BIND,
	SM_ERR_LISTEN,
	SM_ERR_ADD_SOCKET,
	SM_ERR_ALLOC_SOCKETSET,
	SM_ERR_CHECK_SOCKETS,
	SM_ERR_GET_MESSAGE,
	SM_ERR_PUT_MESSAGE,

	SM_RESOLVE_HOST,
	SM_STARTING_SERVER,
	SM_STOPPING_SERVER,
	SM_STARTING_CLIENT,
	SM_STOPPING_CLIENT,
	SM_CHECK_NICK,
	SM_DUPLICATE_ID,
	SM_ADD_CLIENT,
	SM_KILL_CLIENT,
	SM_GET_MESSAGE,
	CMD_UNSPECIFIED,
	CMD_NICK,
	CMD_SRV_MSG,
	CMD_MESSAGE,
	CMD_REQ_GAME,
	CMD_SND_GAME,
	CMD_LOCAL_SOCKET,
	CMD_LOCAL_PL,
	CMD_SHOW_FOW,
	CMD_RUN_GAME,
	CMD_END_TURN,
	CMD_CITY_PROD,
	CMD_CITY_RES,
	CMD_MOVE_UNITS,
	CMD_DISBAND_UNIT,
	CMD_DISBAND_UNITS,
*/
uint32_t Game::server_listener(aSocket *s,uint32_t st,intptr_t p1,intptr_t p2,intptr_t p3) {
	return app.handle_server_event(s,st,p1,p2,p3);
}

uint32_t Game::handle_server_event(aSocket *s,uint32_t st,intptr_t p1,intptr_t p2,intptr_t p3) {
debug_output("Game::handle_server_event(%s)\n",aSocket::message_names[st]);
	switch(st) {
		case SM_ERR_BIND:break; // Cannot host server, there is a server active alreday.
		case SM_ERR_RESOLVE_HOST:
		case SM_ERR_OPEN_SOCKET:
		case SM_ERR_CONNECT:
		case SM_ERR_LISTEN:
		case SM_ERR_ADD_SOCKET:
//		case SM_ERR_ALLOC_SOCKETSET:
//		case SM_ERR_CHECK_SOCKETS:
//		case SM_ERR_GET_MESSAGE:
//		case SM_ERR_PUT_MESSAGE:
handleMessage(ACTION_PRINT,MSG_WARNING,"Game::handle_server_event(error=%u[%s],message=%s)",st,aSocket::message_names[st],(const char *)p2);
			break;
		case SM_CHECK_NICK:debug_output("Check nick: %s\n",*(char **)p1);return 1;
		case SM_DUPLICATE_ID:debug_output("Game::server_listener(SM_DUPLICATE_ID)\n");return 1;
		case SM_STARTING_SERVER:
		{
fprintf(stderr,"Game::handle_server_event(lock)\n");
fflush(stderr);
//			gdk_threads_enter();
//			lock();
			uint32_t ipaddr = server.getIP();
			handleMessage(ACTION_PRINT,MSG_INFO,_("Starting server..."));
			handleMessage(ACTION_PRINT,MSG_INFO,_("IP address: %d.%d.%d.%d"),
					ipaddr>>24,(ipaddr>>16)&0xff,(ipaddr>>8)&0xff,ipaddr&0xff);
			handleMessage(ACTION_PRINT,MSG_INFO,_("Port: %d"),server.getPort());
fprintf(stderr,"Game::handle_server_event(unlock)\n");
fflush(stderr);
//			gdk_threads_leave();
//			unlock();
			break;
		}
		case SM_STOPPING_SERVER:handleMessage(ACTION_PRINT,MSG_INFO,_("Stopping server..."));break;
		case SM_ADD_CLIENT:
		{
			aConnection c = (aConnection)p1;
			sendServerCommand(c,CMD_WELCOME,0);
			break;
		}
		case SM_KILL_CLIENT:
		{
fprintf(stderr,"Game::handle_server_event(lock)\n");
fflush(stderr);
			gdk_threads_enter();
//			lock();
debug_output("Kill Client.\n");
			aConnection c = (aConnection)p1;
			c->setActive(false);
			sendServerCommand(0,CMD_KILL_CLIENT,c->getID());
fprintf(stderr,"Game::handle_server_event(unlock)\n");
fflush(stderr);
			gdk_threads_leave();
//			unlock();
			break;
		}
		case SM_GET_MESSAGE:
		{
			aConnection c = (aConnection)p1;
			uint8_t *data = (uint8_t *)p2,cmd;
			int len;
			len = (int)p3;
			unpack_header(&data,cmd);
debug_output("Server message [%d] %s ID[%d,%d]\n",(int)len,cmd_names[cmd],c->getID(),client.getID());
fprintf(stderr,"Game::handle_server_event(lock)\n");
fflush(stderr);
			gdk_threads_enter();
//			lock();
			switch(cmd) {
				case CMD_TEXT_MESSAGE:
				{
					data += 5;
debug_output("Server receive message: %s\n",(const char *)data);
					server.send((uint8_t *)p2,(size_t)p3);
					break;
				}
				case CMD_REQUEST_CLIENTS:
					sendClients(c);
					break;
				case CMD_REQUEST_GAME:
					if(c->getID()==client.getID()) {
						c->setActive(true);
						handleMessage(ACTION_PRINT,MSG_SERVER,_("Welcome %s! You are hosting this game."),c->getNick());
					} else {
						sendGame(c);
					}
					break;
				case CMD_ACTIVATE_CLIENT:
					sendClient(c);
					c->setActive(true);
					sendServerCommand(0,CMD_ACTIVATE_CLIENT,c->getID());
					break;
				default:server.send((uint8_t *)p2,(size_t)p3);break;
			}
fprintf(stderr,"Game::handle_server_event(unlock)\n");
fflush(stderr);
			gdk_threads_leave();
//			unlock();
			break;
		}
	}
	return 0;
}

uint32_t Game::client_listener(aSocket *s,uint32_t st,intptr_t p1,intptr_t p2,intptr_t p3) {
	return app.handle_client_event(s,st,p1,p2,p3);
}

uint32_t Game::handle_client_event(aSocket *s,uint32_t st,intptr_t p1,intptr_t p2,intptr_t p3) {
debug_output("Game::handle_client_event(%s)\n",aSocket::message_names[st]);
	switch(st) {
		case SM_ERR_RESOLVE_HOST:
		case SM_ERR_OPEN_SOCKET:
		case SM_ERR_CONNECT:
		case SM_ERR_ADD_SOCKET:
//		case SM_ERR_ALLOC_SOCKETSET:
//		case SM_ERR_CHECK_SOCKETS:
//		case SM_ERR_GET_MESSAGE:
//		case SM_ERR_PUT_MESSAGE:
handleMessage(ACTION_PRINT,MSG_WARNING,"Game::handle_client_event(error=%u[%s],message=%s)",st,aSocket::message_names[st],(const char *)p2);
			break;
		case SM_STARTING_CLIENT:
		{
fprintf(stderr,"Game::handle_client_event(lock)\n");
fflush(stderr);
//			gdk_threads_enter();
//			lock();
			uint32_t ipaddr = client.getIP();
			const char *host = client.getHost();
			handleMessage(ACTION_PRINT,MSG_INFO,"Connecting to server...");
			handleMessage(ACTION_PRINT,MSG_INFO,"IP Address: %d.%d.%d.%d",
				ipaddr>>24,(ipaddr>>16)&0xff,(ipaddr>>8)&0xff,ipaddr&0xff);
			handleMessage(ACTION_PRINT,MSG_INFO,"Hostname: %s",host? host : "N/A");
			handleMessage(ACTION_PRINT,MSG_INFO,"Port: %d",client.getPort());
fprintf(stderr,"Game::handle_client_event(unlock)\n");
fflush(stderr);
//			gdk_threads_leave();
//			unlock();
			break;
		}
		case SM_STOPPING_CLIENT:handleMessage(ACTION_PRINT,MSG_INFO,"Disconnecting from server...");break;
		case SM_GET_MESSAGE:
		{
			uint8_t *data = (uint8_t *)p2,cmd;
			int len;
			len = (int)p3;
			unpack_header(&data,cmd);
fprintf(stderr,"Game::handle_client_event(lock)\n");
fflush(stderr);
			gdk_threads_enter();
//			lock();
debug_output("Client message [%d] %s\n",(int)len,cmd_names[cmd]);
			switch(cmd) {
				case CMD_WELCOME:sendClientCommand(CMD_REQUEST_CLIENTS);break;
				case CMD_TEXT_MESSAGE:
				{
					uint32_t cid;
					uint8_t type;
					unpack_uint32(&data,cid);
					unpack_uint8(&data,type);
debug_output("Client receive message: %s\n",(const char *)data);
					handleAction(ACTION_PRINT,type,(intptr_t)data,cid);
					break;
				}
				case CMD_SEND_CLIENTS:
					if(receiveClients((uint8_t *)p2)>0) {
						sendClientCommand(CMD_ACTIVATE_CLIENT);
						sendClientCommand(CMD_REQUEST_GAME);
					}
					break;
				case CMD_ACTIVATE_CLIENT:
				{
					int v = get_unpack_int32(data);
					Player *pl = (Player *)clients.get(v);
					if(pl) handleMessage(ACTION_PRINT,MSG_SERVER,_("%s joined."),pl->nick);
					break;
				}
				case CMD_KILL_CLIENT:
				{
					int v = get_unpack_int32(data);
debug_output("Client kill: %d\n",v);
					Player *pl = (Player *)clients.remove(v);
					if(pl) {
						pl->status |= PL_AI_PASSIVE;
//						players.remove(pl->nick);
debug_output("Client kill: %s\n",pl->nick);
						handleMessage(ACTION_PRINT,MSG_SERVER,_("%s left."),pl->nick);
//						delete pl;
						if(setup.dialog) setup.dialog->updatePlayers();
					}
					break;
				}
				case CMD_SEND_GAME:
debug_output("Client CMD_SEND_GAME\n");
					unpackGame((uint8_t *)p2);
					if(setup.dialog) delete setup.dialog;
					break;
				case CMD_START_GAME:
				{
					int n = get_unpack_int32(data);
					if(n) {
						firstUnit();
						startGame();
					} else stopGame();
					break;
				}
				case CMD_SHOW_FOW:setShowFogOfWar(get_unpack_int32(data));break;
				case CMD_BEGIN_TURN:beginTurn(get_unpack_int32(data));break;
				case CMD_END_TURN:
				{
					int n = get_unpack_uint32(data);
					Player *p = (Player *)clients.get(n);
					if(p) endTurn(p);
					break;
				}
				case CMD_PLAYER_CLAIM_CITY:
				{
					uint32_t pl,c;
					uint8_t alliance;
					unpack_uint32(&data,pl);
					unpack_uint32(&data,c);
					unpack_uint8(&data,alliance);
					handleAction(ACTION_PLAYER_CLAIM_CITY,pl,c,alliance);
					break;
				}
				case CMD_CITY_PRODUCTION:
				{
					int32_t x,y,n;
					unpack_int32(&data,x);
					unpack_int32(&data,y);
					unpack_int32(&data,n);
					setCityProduction(x,y,n);
					break;
				}
				case CMD_MOVE_UNIT:
				{
					int32_t x1,y1,x2,y2;
					unpack_int32(&data,x1);
					unpack_int32(&data,y1);
					unpack_int32(&data,x2);
					unpack_int32(&data,y2);
					moveUnit(x1,y1,x2,y2);
					break;
				}
				case CMD_DISBAND_UNIT:
				{
					int32_t n;
					Unit *u;
					unpack_int32(&data,n);
					uint32_t id;
					for(int i=0; i<n; i++) {
						unpack_uint32(&data,id);
						u = (Unit *)units.get(id);
						if(u) killUnit(u);
					}
					updateEconomy();
					break;
				}
			}
fprintf(stderr,"Game::handle_client_event(unlock)\n");
fflush(stderr);
			gdk_threads_leave();
//			unlock();
			break;
		}
	}
	return 0;
}



