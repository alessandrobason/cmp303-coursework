#pragma once

#include <string>

#include <socket.h>
#include <cthreads.h>
#include <tracelog.h>
#include <str.h>

#include <utils/vec.h>

#include "net_types.h"
#include "convert.h"

constexpr int SEND_ALL = -1;

struct Net {
    static constexpr int max_conn = 10;

    void init(bool is_server);
    void initServer();
    void initClient();

    void cleanup();

    void acceptConnection(int index);
    void closeConnection(int index);
    bool getMessage(int index);

    std::string getGlobalIp();
    std::string getLocalIp();

    template<typename T>
    int sendMessage(const Message<T> &msg, int index = 0) {
        lock_t lock(mutex);
        
        Message<char[sizeof(msg.data)]> message;
        message.type = msg.type;
        convertToNet(msg.data, message.data);
        
        if (index == SEND_ALL) {
            int count = 0;
            for (int i = 1; i < conn_count; ++i) {
                count += skSend(sockets[i], &message, sizeof(message));
            }
            return count;
        }
        else {
            return skSend(sockets[index], &message, sizeof(message));
        }
    }

    template<typename T>
    Message<T> getMessage(int type, int index = 0) {
        Message<T> msg;
        msg.type = type;

        Message<char[sizeof(T)]> message;

        lock_t lock(mutex);
        int count = skReceive(sockets[index], &message.data, sizeof(message.data));
        if(count == SOCKET_ERROR) {
            int err = skGetError();
            if(err != WSAEWOULDBLOCK) {
                warn("Client connection closed or broken");
            }
            else {
                warn("would block");
            }
        }

        convertToHost(message.data, msg.data);

        return msg;
    }

    inline bool isServer() { return is_server; }
    inline bool isClient() { return !is_server; }
    inline bool isReady()  { lock_t lock(mutex); return is_ready; }
    inline void setReady(bool ready) { lock_t lock(mutex); is_ready = ready; }

    str_t username;
    std::string ip = "127.0.0.1";
    socket_t sockets[max_conn + 1];
    WSAEVENT events[max_conn + 1];
    bool writable[max_conn + 1];
    int conn_count = 1;
    WSANETWORKEVENTS network_events;
    bool is_server = false;
    bool is_ready = false;
    bool should_close = false;
    cthread_t thread;
    cmutex_t mutex;
};

extern Net net;