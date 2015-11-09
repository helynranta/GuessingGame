#ifndef _TCPServer_HPP
#define _TCPServer_HPP

#include <iostream>
#include <vector>
#include <string>

#include <unistd.h> // close
#include <netdb.h> // getprotobyname
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <arpa/inet.h> // inet_ntoa

#include "Message.hpp"

enum STATE {RUNNING, EXIT};

class Connection {
private:
	/* private data */
	int id = -1;
	std::string nick = "undefined";
	int TCPSock = -1;
	struct sockaddr_in info;
	socklen_t sock_len;
protected:
	/* protected data */
public:
	inline Connection(int i, int t) : id(i), TCPSock(t) {
		nick = "User"+std::to_string(id);
	}
	inline ~Connection() {}
	//* getters *//
	inline int getID() const { return id; }
	inline int getTCPSock() const { return TCPSock; }
	inline struct sockaddr_in* getAddr() { return &info; }
	inline std::string getNick() const { return nick; }
	//* setters *//
	inline void setNick(std::string n) { nick = n; }
	inline void setAddr(struct sockaddr_in in, socklen_t t) { info = in; sock_len = t; }
	int tries = 0;
};

class TCPServer {
private:
    /* private data */
    const int PORT = 5200;
    const int BACKLOG = 10; // how many pending connections queue will hold

    STATE m_state = STATE::EXIT;
    int sockfd_tcp = 0; // for listen
	int sockfd_udp = 0;
    int new_fd  = 0; // for new connections
    struct sockaddr_in my_addr;
    struct timeval tv;  // time wait for socket to be changed
    fd_set readfds, writefds, exceptfds;
    std::vector<Connection> connected;
    std::vector<Message> messages_in;
    std::vector<Message> messages_out;
protected:
    /* protected data */
    bool init(void);
	void select(void);
	void logic(void);
	// our random number
	int random_number = rand() % 10;
public:
    /* public data */
    TCPServer(void);
    virtual ~TCPServer (void);
    void run(void);
    void destroy(void);
};

#endif
