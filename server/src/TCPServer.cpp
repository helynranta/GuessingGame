#include "../inc/TCPServer.hpp"
/* Constructor: TCPSserver */
TCPServer::TCPServer(void) {

}
/* Deconstructor: ~TCPSserver */
TCPServer::~TCPServer (void) {

}
/* run: main loop for server */
// http://stackoverflow.com/questions/2284428/in-c-networking-using-select-do-i-first-have-to-listen-and-accept
void TCPServer::run(void) {
    // max is the biggest possible socket id for select
    int l_max = 0;
    char buffer[1024];
    std::string new_id_msg;
    if(init()) {
        std::cout << "--------SERVER RUNNING----------" << std::endl;
        m_state = STATE::RUNNING;
        while(m_state == STATE::RUNNING) {
            // put sockfd to set of file descriptors to read from
            FD_ZERO(&readfds);
            FD_SET(sockfd, &readfds);
            if(sockfd >= l_max) l_max = sockfd +1;
            // add all connections to fd_set
            for( auto& it : connected ) {
                FD_SET(it.getTCPSock(), &readfds);
                if(it.getTCPSock() >= l_max) l_max = it.getTCPSock() + 1;
            }
            tv.tv_sec = 2; tv.tv_usec = 0;
            //std::cout << "ping" << std::endl;
            if(select(l_max, &readfds, nullptr, nullptr, &tv) > 0) {
                // readable
                if (FD_ISSET(sockfd, &readfds)) {
                    connected.emplace_back(++ID_COUNT, ::accept(sockfd, static_cast<struct sockaddr*>(NULL), NULL));
                    std::cout << "new client connected with TCP!" << std::endl;
                    std::cout << "new client was given sock " << connected.back().getID() << " and new id " << connected.back().getTCPSock() << std::endl;
                    std::cout << "amount connected clients now: " << connected.size() << std::endl;
                    new_id_msg = "newid:"+std::to_string(connected.back().getID());
                    std::cout << new_id_msg << std::endl;
                    ::sendto(connected.back().getTCPSock(), new_id_msg.c_str(), sizeof(new_id_msg.c_str()), NULL, NULL, NULL);
                    for(auto& sit : connected) {
                        send(sit.getTCPSock(), ("srvmsg:New client connected, User"+std::to_string(connected.back().getID())).c_str(), 1024, 0);
                    }
                }
                for( auto& rit : connected ) {
                    //std::cout << "lol" << std::endl;
                    memset(buffer, '\0', sizeof(buffer));
                    if(FD_ISSET(rit.getTCPSock(), &readfds)) {
                        recv(rit.getTCPSock(), buffer, sizeof(buffer), 0);
                        //std::cout << buffer << std::endl;
                        if(strlen(buffer) == 0) {
                            std::cout << "TCP connection has disconnected!" << std::endl;
                            rit = connected.back();
                            connected.pop_back();
                            std::cout << "amount connected clients now: " << connected.size() << std::endl;
                        } else {
                            // send to all TCP connections
                            //std::cout << buffer << std::endl;
                            std::string clmsg(buffer);
                            for(auto& sit : connected) {
                                send(sit.getTCPSock(), ("clmsg:"+std::to_string(rit.getID())+":"+clmsg).c_str(), 1024, 0);
                            }
                        }
                    }
                }
            }
        }
    }
}
/* destroy */
void TCPServer::destroy(void) {
    close(new_fd);
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
    if( (sockfd = socket(AF_INET, SOCK_STREAM, getprotobyname("tcp")->p_proto)) == -1 ) {
        std::cerr << "Socket error" << std::endl;
        success = false;
    }
    int reuse = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sockfd, reinterpret_cast<struct sockaddr*>(&my_addr), sizeof(struct sockaddr_in)) == -1) {
        std::cerr << "Bind error" << std::endl;
        success = false;
    } else {
        std::cout << "bind successfull:" << std::endl;
    }
    if(listen(sockfd, BACKLOG) == -1) {
        std::cerr << "listen error" << std::endl;
        success = false;
    } else {
        std::cout << "listen successfull:" << std::endl;
    }

    return success;
}
