#include "net.h"

#include <stdio.h>
#include <os.h>

#include <gameplay/map_scene.h>

// The UDP port number on the server
#define SERVERPORT 4444

Net net;

static int die(const char *msg);

static int serverFun(void *args);
static int clientFun(void *args);

void Net::init(bool is_server) {
    username = getUserName();
    // in the rare case getUserName should fail, fall back to a random username
    if(!username.buf) {
        username = strInitStr(TextFormat("anon_%d", rand() % 10'000));
    }

    debug("username: %s", username.buf);

    socket_t &sock = sockets[0];
    sock = skOpen(SOCK_TCP);
    if(!skIsValid(sock)) {
        die("couldn't open socket");
    }

    if(is_server) initServer();
    else          initClient();
}

void Net::initServer() {
    socket_t &listen = sockets[0];
    WSAEVENT &event = events[0];

    if(!skBind(listen, ip.c_str(), SERVERPORT)) {
        die("couldn't bind socket");
    }
    info("Server socket bound to address %s, port %d", ip.c_str(), SERVERPORT);

    event = WSACreateEvent();
    if(event == WSA_INVALID_EVENT) {
        die("Server couldn't create event");
    }
	//Assosciate this event with the socket types we're interested in
	//In this case, on the server, we're interested in Accepts and Closes
    WSAEventSelect(listen, event, FD_ACCEPT | FD_CLOSE);

	//Start listening for connection requests on the socket
    if(!skListen(listen)) {
        die("Couldn't listen on socket");
    }

    info("Listening on socket...");

    mutex = mtxInit();

    thread = thrCreate(serverFun, this);
    if(!thrValid(thread)) {
        fatal("Couldn't create cserver thread");
    }
}

void Net::initClient() {
    mutex = mtxInit();

    thread = thrCreate(clientFun, this);
    if(!thrValid(thread)) {
        fatal("Couldn't create client thread");
    }
}

void Net::cleanup() {
    strFree(&username);

    {
        lock_t lock(mutex);
        should_close = true;
    }
    if(!thrJoin(thread, NULL)) {
        fatal("couldn't join thread");
    }
    mtxDestroy(mutex);
    if(!skClose(sockets[0])) {
        die("couldn't close socket");
    }
    if(!skCleanup()) {
        die("coudln't clean up sockets");
    }
}

void Net::update() {
    // DWORD return_val = WSAWaitForMultipleEvents(conn_count, events, false, 0, false);
}

void Net::acceptConnection(int index) {
    lock_t lock(mutex);

    int error = network_events.iErrorCode[FD_ACCEPT_BIT];
    if(error) {
        fatal("FD_ACCEPT failed with error %d", error);
    }

    // Accept a new connection and add it to the socket and event lists
    sockets[conn_count] = skAccept(sockets[0]);
    events[conn_count] = WSACreateEvent();

    if(events[conn_count] == WSA_INVALID_EVENT) {
        die("couldn't create event");
    }

    if(WSAEventSelect(
            sockets[conn_count], events[conn_count], 
            FD_CLOSE | FD_WRITE | FD_READ
       ) == SOCKET_ERROR) {
        die("couldn't select event");
    }

    info("Socket %d connected", sockets[conn_count]);
    conn_count++;
}

void Net::closeConnection(int index) {
    lock_t lock(mutex);

	int error = network_events.iErrorCode[FD_CLOSE_BIT];
	//We ignore the error if the client just force quit
    if(error && error != WSAECONNABORTED) {
        fatal("FD_CLOSE failed with error %d", error);
    }

    // Close connection
    if(skClose(sockets[index])) {
        info("Successfully closed socket %d", sockets[index]);
    }
    else {
        err("Couldn't close socket %d => %s", sockets[index], skGetErrorString());
    }

    if(!WSACloseEvent(events[index])) {
        err("Couldn't close event %d => %s", index, skGetErrorString());
    }

    conn_count--;

	// swap with last one
	sockets[index] = sockets[conn_count];
	events[index] = events[conn_count];
}

bool Net::getMessage(int index) {
    u8 type;
    int count = skReceive(sockets[index], &type, sizeof(type));

    if(count == SOCKET_ERROR) {
        int err = skGetError();

        if(err != WSAEWOULDBLOCK) {
            fatal("Client connection closed or broken");
            AskCloseWindow();
            return false;
        }
        warn("would block");
        return false;
    }
    else {
        switch(type) {
        case MSG_READY: {
            auto msg = getMessage<u8>(type, index);
            gmap.setMapId(msg.data);
            lock_t lock(mutex);
            is_ready = true;
            break;
        }
        case MSG_NEW_PLAYER: {
            auto msg = getMessage<PlayerNew>(type, index);
            players[msg.data.id].addPlayer(msg.data.id, msg.data.is_main);
            break;
        }
        case MSG_PLAYER_NAME: {
            auto msg = getMessage<PlayerName>(type, index);
            players[msg.data.id].setName(msg.data.name);
            break;
        }
        case MSG_POS: {
            auto msg = getMessage<PlayerPos>(type, index);
            players[msg.data.id].setPosition(msg.data.pos);
            break;
        }
        case MSG_ANIM: {
            auto msg = getMessage<PlayerAnim>(type, index);
            players[msg.data.id].setAnimation(msg.data.index);
            break;
        }
        case MSG_STATE: {
            auto msg = getMessage<PlayerAnim>(type, index);
            players[msg.data.id].setState(msg.data.index);
            break;
        }
        case MSG_OBJ_ADD: {
            auto msg = getMessage<AddRemMsg>(type, index);
            gmap.addObject(msg.data.id, msg.data.pos);
            break;
        }
        case MSG_OBJ_REM: {
            auto msg = getMessage<AddRemMsg>(type, index);
            gmap.removeObject(msg.data.id, msg.data.pos);
            break;
        }
        case MSG_POW_ADD: {
            auto msg = getMessage<AddPowerup>(type, index);
            gmap.addObject(Powerup::id(), msg.data.pos, msg.data.type);
            break;
        }
        default:
            err("unknown message type: %d", type);
            AskCloseWindow();
        }
    }

    return false;
}

// from https://stackoverflow.com/questions/49335001/get-local-ip-address-in-c
std::string Net::getLocalIp() {
    /* Try to connect to google's dns (8.8.8.8) using a udp socket,
     * use getsockname to get local ip
     */

    socket_t sock = skOpen(SOCK_UDP);
    if (sock == INVALID_SOCKET) {
        die("couldn't get local ip");
        return "";
    }

    if (!skConnect(sock, "8.8.8.8", 53)) {
        die("couldn't connect to google dns");
        return "";
    }

    sk_addrin_t name{};
    sk_len_t namelen = sizeof(name);
    int err = getsockname(sock, (sk_addr_t *)&name, &namelen);

    char buffer[80];
    const char *p = inet_ntop(AF_INET, &name.sin_addr, buffer, 80);

    if (!p) {
        die("couldn't get ip from sk_addrin_t");
        return "";
    }

    return p;
}

static int die(const char *msg) {
    fatal("%s > %s", msg, skGetErrorString());
    return 1;
}

static int serverFun(void *args) {
    while(true) {
        {
            lock_t lock(net.mutex);
            if(net.should_close) break;
        }
        DWORD return_val = WSAWaitForMultipleEvents(net.conn_count, net.events, false, WSA_INFINITE, false);
    
        if(return_val != WSA_WAIT_FAILED) {
            // In practice, event_index will equal return_val, but this is here for compatability
            int event_index = return_val - WSA_WAIT_EVENT_0;
            if(WSAEnumNetworkEvents(
                net.sockets[event_index],
                net.events[event_index],
                &net.network_events
            )) {
                return die("Retrieving event information failed");
            }

            long event = net.network_events.lNetworkEvents;

            if(event & FD_CLOSE) {
                net.closeConnection(event_index);
            }

			if(event & FD_ACCEPT) {
				net.acceptConnection(event_index);
			}

			if(event & FD_READ) {
				net.getMessage(event_index);
			}

			if(event & FD_WRITE) {
                lock_t lock(net.mutex);
				net.writable[event_index] = true;
			}
        }
        else {
            return die("WSAWaitForMultipleEvents failed!");
        }
    }

    return 0;
}

static int clientFun(void *args) {
    while (!skConnect(net.sockets[0], net.ip.c_str(), SERVERPORT)) {
        warn("couldn't connect to server, retrying...");
    }

    info("Connected to server");

    // send username
    Message<std::array<char, 8>> msg;
    msg.type = MSG_PLAYER_NAME;
    memcpy(&msg.data[0], net.username.buf, sizeof(msg.data));

    net.sendMessage(msg);
    net.setReady(true);

    while(true) {
        {
            lock_t lock(net.mutex);
            if(net.should_close) break;
        }
        
        net.getMessage(0);
    }

    return 0;
}