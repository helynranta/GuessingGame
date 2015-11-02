#include <sys/select.h>

#include "Inet/InetConnection.hpp"

void InetConnection::init(void) {

}
void InetConnection::destroy(void)  {
    // delete messages behind pointers
    for ( auto& it : messages) {
        delete it;
    }
    // empty whole vector
    messages.empty();
}
bool InetConnection::sendTCP(const std::string& msg) {
    bool success = false;

    if((::sendto(sockettcp, msg.c_str(),strlen(msg.c_str()), 0, reinterpret_cast<struct sockaddr *>(&me_addr), sizeof(me_addr))) < 0) {
        std::cout << "error sendto()" << std::endl;
    }
    else {
        //std::cout << "TCP: Sent msg: " << msg << std::endl;
        success = true;
    }

    return success;
}
bool InetConnection::sendUDP(const std::string& msg) {
  bool success = false;
  if((::sendto(socketudp, msg.c_str(), sizeof(msg.c_str()), 0, reinterpret_cast<struct sockaddr *>(&server), sizeof(server))) < 0) {
    std::cerr << "UPD Send error" << std::endl;
  } else {
    //std::cout << "Client: send UPD msg: " << msg << std::endl;
    success = true;
  }
  return success;
}

bool InetConnection::connect(std::string l_ip, unsigned int l_port) {
    //getaddrinfo(l_ip.c_str(), l_ip.c_str(), &hints, &res);
    server.sin_family = AF_INET;
    server.sin_port = htons(l_port);
    server.sin_addr.s_addr = inet_addr(l_ip.c_str());
    // other inits
    bool success = true;
    m_state = ConnectionState::CONNECTING;
    std::cout << "Trying to connect to host: " << l_ip << ":" << l_port << std::endl;
    // connect with TCP
    if((sockettcp = socket(AF_INET, SOCK_STREAM, 0))<0) {
        std::cerr << "NO SOCKET FOR YOU BIATCH!"<< std::endl;
        success = false;
    }
    //try to bind
    memset( reinterpret_cast<char*>(&me_addr), 0, sizeof(me_addr));
    me_addr.sin_family = AF_INET;
    me_addr.sin_port   = htons(3200);
    me_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    for(unsigned int i = 1024; i < 6500; i++) {
        me_addr.sin_port   = htons(i);
        if(::bind(sockettcp, reinterpret_cast<struct sockaddr*>(&me_addr), sizeof(me_addr)) >= 0) {
            break;
        }
    }

    // THIS IS BLOCKING!
    if(::connect(sockettcp,reinterpret_cast<struct sockaddr*>(&server),sizeof(me_addr)) < 0) {
        std::cerr << "Cannot connect..." << std::endl;
        m_state = ConnectionState::DISCONNECTED;
    }
    else {
        std::cout << "Connection to "<< l_ip <<" was succesfull!" << std::endl;
        m_state = ConnectionState::CONNECTED;
        ip = l_ip;
        port = l_port;
    }
    // create udp socket
    if((socketudp = socket(AF_INET, SOCK_DGRAM, 0 ))<0) {
        std::cerr << "no upd socket for you, BIATCH!" << std::endl;
        success = false;
    }
    // fill me, because why the fuck not
    if(::bind(socketudp, reinterpret_cast<struct sockaddr*>(&me_addr), sizeof(me_addr)) < 0) {
      std::cerr << "no udp bind" << std::endl;
    }
    return success;
}
bool InetConnection::disconnect() {

  //close(sockettcp);
  return true;
}
std::string InetConnection::update() {
    // http://stackoverflow.com/questions/13878306/multiplexing-stdin-and-socket-in-client-c
    std::string msg = "";
    struct timeval tv;
    char buffer[1024];
    memset(buffer, '\0', sizeof(buffer));
    FD_ZERO(&rset);
    FD_SET(sockettcp, &rset);
    FD_SET(socketudp, &rset);
    int l_max = std::max(sockettcp, socketudp);
    tv.tv_sec = long(0.01);
    if(select(l_max+1, &rset, nullptr, nullptr, &tv) > 0) {
        if(FD_ISSET(sockettcp, &rset)) {
            memset(buffer, '\0', sizeof(buffer));
            recv(sockettcp, buffer, sizeof(buffer), 0);
            //std::cout << buffer << std::endl;
            if(strlen(buffer) == 0) {
                // std::cout << "server disconnected!" << std::endl;
            } else {
                msg = buffer;
            }
        }
        if (FD_ISSET(socketudp, &rset)) {
            memset(buffer, '\0', sizeof(buffer));
            recv(socketudp, buffer, sizeof(buffer), 0);
            if(strlen(buffer) == 0) {
                // std::cout << "server disconnected!" << std::endl;
            } else {
                msg = buffer;
            }
        }
    }
    return msg;
}
