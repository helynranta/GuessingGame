#include <sys/select.h>

#include "Inet/InetConnection.hpp"

ConnectionState   InetConnection::m_state = ConnectionState::DISCONNECTED;

struct sockaddr_in  InetConnection::me_tcp;
struct sockaddr_in  InetConnection::me_udp;
int               InetConnection::socketfd = -1;
fd_set InetConnection::rset;
std::string       InetConnection::ip = "";
unsigned int       InetConnection::port = 0;
int InetConnection::socketudp = 0;


std::vector<Message*> InetConnection::messages;

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

    if((::sendto(socketfd, msg.c_str(),strlen(msg.c_str()), 0, reinterpret_cast<struct sockaddr *>(&me_tcp), sizeof(me_tcp))) < 0) {
        std::cout << "error sendto()" << std::endl;
    }
    else {
        std::cout << "TCP: Sent msg: " << msg << std::endl;
        success = true;
    }

    return success;
}
bool InetConnection::sendUDP(const std::string& msg) {
  bool success = false;
  if((::sendto(socketudp, msg.c_str(), sizeof(msg.c_str()), 0, reinterpret_cast<struct sockaddr *>(&me_udp), sizeof(me_udp))) < 0) {
    std::cerr << "UPD Send error" << std::endl;
  } else {
    std::cout << "Client: send UPD msg: " << msg << std::endl;
    success = true;
  }
  return success;
}

bool InetConnection::connectTCP(std::string l_ip, unsigned int l_port) {
    bool success = true;
    m_state = ConnectionState::CONNECTING;
    std::cout << "Trying to connect to host: " << l_ip << ":" << l_port << std::endl;
    // connect with TCP
    if((socketfd = socket(AF_INET, SOCK_STREAM, 0))<0) {
        std::cerr << "NO SOCKET FOR YOU BIATCH!"<< std::endl;
        success = false;
    }
    memset( reinterpret_cast<char*>(&me_tcp), 0, sizeof(me_tcp));
    me_tcp.sin_family =AF_INET;
    me_tcp.sin_port   = htons(l_port);
    me_tcp.sin_addr.s_addr = inet_addr(l_ip.c_str());
    // lets just bind first
    //if(bind(socketfd, reinterpret_cast<struct sockaddr*>(&me_tcp), sizeof(me_tcp)) < 0) {
    //    std::cerr << "no bind lol" << std::endl;
    //}

    // THIS IS BLOCKING!
    if(::connect(socketfd,reinterpret_cast<struct sockaddr*>(&me_tcp),sizeof(me_tcp)) < 0) {
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
    memset( reinterpret_cast<char*>(&me_udp), 0, sizeof(me_udp));
    me_udp.sin_family =AF_INET;
    me_udp.sin_port   = htons(l_port);
    me_udp.sin_addr.s_addr = inet_addr(l_ip.c_str());
    if((socketudp = socket(AF_INET, SOCK_DGRAM, 0 ))<0) {
      std::cerr << "no upd socket for you, BIATCH!" << std::endl;
      success = false;
    }
    // fill me, because why the fuck not
    //if(bind(socketudp, reinterpret_cast<struct sockaddr*>(&me_udp), sizeof(me_udp)) <0) {
    //  std::cerr << "no bind lol" << std::endl;
    //}
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
    tv.tv_sec = long(0.01); tv.tv_usec = long(0.001);
    if(select(socketfd+1, &rset, nullptr, nullptr, &tv) > 0) {
        if(FD_ISSET(socketfd, &rset)) {
            memset(buffer, '\0', sizeof(buffer));
            recv(socketfd, buffer, sizeof(buffer), 0);
            //std::cout << buffer << std::endl;
            if(strlen(buffer) == 0) {
                // std::cout << "server disconnected!" << std::endl;
            } else {
                msg = buffer;
            }
        }
    }
    return msg;
}
