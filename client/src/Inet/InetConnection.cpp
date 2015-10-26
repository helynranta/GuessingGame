#include <sys/select.h>

#include "Inet/InetConnection.hpp"

ConnectionState   InetConnection::m_state = ConnectionState::DISCONNECTED;

struct sockaddr_in  InetConnection::server;
int               InetConnection::socketfd = -1;
fd_set InetConnection::rset;
std::string       InetConnection::ip = "";
unsigned int       InetConnection::port = 0;


std::vector<Message*> InetConnection::messages;

void InetConnection::init(void) {
    // this is temp. replace 1 with size of message
}

void InetConnection::destroy(void)  {
    // delete messages behind pointers
    for ( auto& it : messages) {
        delete it;
    }
    // empty whole vector
    messages.empty();
}

bool InetConnection::send(const std::string& msg) {
    bool success = false;

    if((::send(socketfd, msg.c_str(),strlen(msg.c_str()), 0)) < 0) {
        std::cout << "error sendto()" << std::endl;
    }
    else {
        //std::cout << "Client: Sent msg: " << msg << std::endl;
        success = true;
    }

    return success;
}

bool InetConnection::connect(std::string l_ip, unsigned int l_port) {
    bool success = true;
    m_state = ConnectionState::CONNECTING;
    std::cout << "Trying to connect to host: " << l_ip << ":" << l_port << std::endl;

    if((socketfd = socket(AF_INET, SOCK_STREAM, 0))<0) {
        std::cerr << "NO SOCKET FOR YOU BIATCH!"<< std::endl;
        success = false;
    }
    server.sin_family=AF_INET;
    server.sin_port = htons(l_port);
    server.sin_addr.s_addr = inet_addr(l_ip.c_str());
    // try to connect
    // THIS IS BLOCKING!
    if(::connect(socketfd,(struct sockaddr*)&server,sizeof(server)) < 0) {
        std::cerr << "Cannot connect..." << std::endl;
        m_state = ConnectionState::DISCONNECTED;
    }
    else {
        std::cout << "Connection to "<< l_ip <<" was succesfull!" << std::endl;
        m_state = ConnectionState::CONNECTED;
        ip = l_ip;
        port = l_port;
    }
    return success;
}
bool InetConnection::disconnect() {

  //close(socketfd);
  return true;
}


std::string InetConnection::update() {
    // http://stackoverflow.com/questions/13878306/multiplexing-stdin-and-socket-in-client-c
    std::string msg = "";
    struct timeval tv;
    char buffer[1024];
    FD_ZERO(&rset);
    FD_SET(socketfd, &rset);
    tv.tv_sec = 0.01; tv.tv_usec = 0.001;
    if(select(socketfd+1, &rset, nullptr, nullptr, &tv) > 0) {
        if(FD_ISSET(socketfd, &rset)) {
            memset(buffer, '\0', sizeof(buffer));
            recv(socketfd, buffer, sizeof(buffer), 0);
            if(strlen(buffer) == 0) {
                std::cout << "server disconnected!" << std::endl;
                m_state = ConnectionState::TIMING_OUT;
            } else {
                msg = buffer;
            }
        }
    }
    return msg;
}
