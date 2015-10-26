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
private:
    static struct sockaddr_in server;
    static int socketfd;
    static int rval;
    static std::string ip;
    static unsigned int port;
    static fd_set rset;
public:
    static ConnectionState m_state;
    static bool sendTCP(const std::string& msg);
    static bool connectTCP(std::string ip, unsigned int port);
    static bool disconnect();
    static std::string update();
    static std::vector<Message*> messages;
    static void init();
    static void destroy();
    static void setState(ConnectionState s) {m_state = s;}
};
#endif
