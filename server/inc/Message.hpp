#ifndef _MESSAGE_HPP
#define _MESSAGE_HPP

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

enum TYPE {
	TCP,
	UDP
};

class Message {
private:
	/* private data */
    int m_tcpsocket = -1;
    int m_udpsocket = -1;
    struct sockaddr_in m_contact; // contact information
    socklen_t m_contact_len = 0;
    TYPE m_type;
	std::string m_message;
protected:
	/* protected data */
public:
	/* public data */
	Message (std::string msg, int tcpsocket);
    Message (std::string msg, int udpsocket, struct sockaddr_in info);
	virtual ~Message();
	bool send(void);
};

#endif
