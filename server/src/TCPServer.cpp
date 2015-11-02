#include "../inc/TCPServer.hpp"

/* Constructor: TCPSserver */
TCPServer::TCPServer(void) {
    srand(time(nullptr));
    random_number = rand()%9+1;
}
/* Deconstructor: ~TCPSserver */
TCPServer::~TCPServer (void) {

}
/* run: main loop for server */
// http://stackoverflow.com/questions/2284428/in-c-networking-using-select-do-i-first-have-to-listen-and-accept
void TCPServer::run(void) {
    if(init()) {
        std::cout << "--------SERVER RUNNING----------" << std::endl;
        m_state = STATE::RUNNING;
        while(m_state == STATE::RUNNING) {
            select();
            logic();
        }
    }
}
void TCPServer::select(void) {
    int l_max = 0; // max is the biggest possible socket id for select
    char buffer[1024]; // one buffer to rule them all
    static int ID_COUNT = 0; // to give them all ids
    // put tcp and upd sockets to set of file descriptors to read from
    FD_ZERO(&readfds); FD_SET(sockfd_tcp, &readfds); FD_SET(sockfd_udp, &readfds);
    if(sockfd_tcp >= l_max) l_max = sockfd_tcp + 1;
    if(sockfd_udp >= l_max) l_max = sockfd_udp + 1;
    // add all connections to fd_set
    for( auto& it : connected ) {
        FD_SET(it.getTCPSock(), &readfds);
        if(it.getTCPSock() >= l_max) l_max = it.getTCPSock() + 1;
    }
    // incoming connection information
    struct sockaddr_in from;
    socklen_t from_size = sizeof from;
    tv.tv_sec = 1; // gibu
    if(::select(l_max, &readfds, nullptr, nullptr, &tv) > 0) { // dat select
        // check for incoming TCP connections
        if (FD_ISSET(sockfd_tcp, &readfds)) {
            memset(buffer, '\0', sizeof(buffer));
            connected.emplace_back(++ID_COUNT, ::accept(sockfd_tcp, reinterpret_cast<struct sockaddr*>(&from), &from_size));
            connected.back().setAddr(from, from_size);
            std::cout << "new client connected with TCP!" << std::endl << "amount connected clients now: " << connected.size() << std::endl;;
            // give client new id
            std::string new_id_msg = "newid:"+std::to_string(connected.back().getID());
            std::cout << new_id_msg << std::endl;
            ::sendto(connected.back().getTCPSock(), new_id_msg.c_str(), sizeof(new_id_msg.c_str()), 0, reinterpret_cast<const struct sockaddr *>(NULL), 0);
            for(auto& sit : connected) {
                if(sit.getID() != ID_COUNT)
                    send(sit.getTCPSock(), ("srvmsg:New client connected, "+connected.back().getNick()).c_str(), 1024, 0);
            }
        }
        // check for incoming UDP diagrams
        if(FD_ISSET(sockfd_udp, &readfds)) {
            memset(buffer, '\0', sizeof(buffer));
            if((::recvfrom(sockfd_udp, buffer, sizeof(buffer), 0, reinterpret_cast<struct sockaddr *>(&from), &from_size)) < 0) {
                std::cerr << "problems recieving udp diagrams" << std::endl;
            }
            // buffer to std::string
            std::string sbuffer(buffer);
            // identify
            int tr = 0;
            Connection* incoming = nullptr;
            for(auto& it : connected) {
                if(atoi(sbuffer.substr(1, sbuffer.length()).c_str()) == it.getID()) {
                    //it.setAddr(from, from_size);
                    incoming = &it;
                    tr = ++it.tries;
                    break;
                }
            }
            if(incoming->tries < 3) {
              int n = atoi(sbuffer.substr(0,1).c_str());
              if(random_number == n) {
                  for(auto& sit : connected) {
                      if(::send(sit.getTCPSock(),("srvmsg:We got a new winner, User"+sbuffer.substr(1,sbuffer.length())).c_str(),1024, 0) < 0) {
                          std::cerr << "problems answering with tcp" << std::endl;
                      }
                      for( auto& it : connected) {
                          it.tries = 0;
                      }
                  }
                  random_number = rand()%9+1;
              } else {
                  std::cout << random_number << "!=" << n << std::endl;
                  if((sendto(sockfd_udp, ("srvmsg:wrong, tried "+std::to_string(tr)+" times").c_str(), 1024, 0, reinterpret_cast<struct sockaddr*>(&from), sizeof(from)))<0) {
                      std::cerr << "server cannot answer with udp..." << std::endl;
                  }
              }
            } else {
              std::cout << "too many tries from " << incoming->getNick() << std::endl;
              from = *incoming->getAddr();
              if((sendto(sockfd_udp, "srvmsg:you have tried too many times", 1024, 0, reinterpret_cast<struct sockaddr*>(&from), from_size))<0) {
                  std::cerr << "server cannot answer with udp..." << std::endl;
              }
            }
        }
        // loop threw old TCP connections
        for(auto& rit : connected) {
            memset(buffer, '\0', sizeof(buffer));
            if(FD_ISSET(rit.getTCPSock(), &readfds)) {
                recvfrom(rit.getTCPSock(), buffer, sizeof(buffer), 0, reinterpret_cast<struct sockaddr*>(&from), NULL);
                if(strlen(buffer) == 0) {
                    rit = connected.back();
                    connected.pop_back();
                    std::cout << "TCP user has disconnected!" << std::endl << "amount connected clients now: " << connected.size() << std::endl;
                } else {
                    // broadcast chat message to all users
                    std::string clmsg(buffer);
                    memset(buffer, '\0', sizeof(buffer));
                    for(auto& sit : connected) {
                        send(sit.getTCPSock(), ("clmsg:"+std::to_string(rit.getID())+":"+clmsg).c_str(), 1024, 0);
                    }
                }
            }
        }
    }
}
void TCPServer::logic(void) {

}
/* destroy */
void TCPServer::destroy(void) {
    for(auto& it : connected) {
      close(it.getTCPSock());
    }
    close(new_fd);
    close(sockfd_tcp);
    close(sockfd_udp);
}
/* init: initializes all server realted things
* @params: void
* @return: bool (success)
*/
bool TCPServer::init(void) {
    std::cout << "--------SERVER INITING----------" << std::endl;
    bool success = true;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
    // try to get socket for server to use
    if( (sockfd_tcp = socket(AF_INET, SOCK_STREAM, getprotobyname("tcp")->p_proto)) == -1 ) {
        std::cerr << "Socket error" << std::endl;
        success = false;
    }
    int reuse = 1;
    setsockopt(sockfd_tcp, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(PORT);
    my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(bind(sockfd_tcp, reinterpret_cast<struct sockaddr*>(&my_addr), sizeof(struct sockaddr_in)) == -1) {
        std::cerr << "Bind error" << std::endl;
        success = false;
    } else {
        std::cout << "bind successfull:" << std::endl;
    }
    if(listen(sockfd_tcp, BACKLOG) == -1) {
        std::cerr << "listen error" << std::endl;
        success = false;
    } else {
        std::cout << "listen successfull:" << std::endl;
    }

    // create udp socket
    if((sockfd_udp = socket(AF_INET, SOCK_DGRAM, 0 ))<0) {
      std::cerr << "no upd socket for you, BIATCH!" << std::endl;
      success = false;
    }
    if(bind(sockfd_udp, reinterpret_cast<struct sockaddr*>(&my_addr), sizeof(my_addr)) <0) {
      std::cerr << "no bind lol" << std::endl;
      success = false;
    }
    return success;
}
