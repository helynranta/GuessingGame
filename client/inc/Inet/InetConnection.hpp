#ifndef _INETCONNECTION_HPP
#define _INETCONNECTION_HPP

#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <iostream>
#include <vector>

#include <SDL2/SDL.h>

#include "Inet/Messages.hpp"

enum ConnectionState { DISCONNECTED, CONNECTING, CONNECTED, TIMING_OUT, EXITING };

class InetConnection {
friend class Engine;
private:
    struct sockaddr_in me_addr;
    struct sockaddr_in server;
    struct sockaddr_in hints; // unused
    struct sockaddr_in res; // unused
    int sockettcp = -1;
    int socketudp = -1;
    int rval;
    std::string ip = "";
    unsigned int port = 0;
    fd_set rset;
protected:
    inline InetConnection() {;}
    inline virtual ~InetConnection() {;}
public:
    ConnectionState m_state = ConnectionState::DISCONNECTED;
    bool sendTCP(const std::string& msg);
    bool sendUDP(const std::string& msg);
    bool connect(std::string ip, unsigned int port);
    bool disconnect();
    std::string update();
    std::vector<Message*> messages;
    void init();
    void destroy();
    void setState(ConnectionState s) {m_state = s;}
};
#endif
