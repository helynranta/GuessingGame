#include "TCPServer.hpp"
/* Constructor: TCPSserver */
TCPServer::TCPServer(void) {

}
/* Deconstructor: ~TCPSserver */
TCPServer::~TCPServer (void) {

}
/* run: main loop for server */
// http://stackoverflow.com/questions/2284428/in-c-networking-using-select-do-i-first-have-to-listen-and-accept
void TCPServer::run(void) {
    int l_max = 0;
    int n=0;
    char buffer[1024];
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
                FD_SET(it, &readfds);
                if(it >= l_max) l_max = it + 1;
            }
            tv.tv_sec = 2; tv.tv_usec = 0;
            //std::cout << "ping" << std::endl;
            if(select(l_max, &readfds, nullptr, nullptr, &tv) > 0) {
                // readable
                if (FD_ISSET(sockfd, &readfds)) {
                    connected.push_back(::accept(sockfd, reinterpret_cast<struct sockaddr*>(NULL), NULL));
                    std::cout << "new client connected!" << std::endl;
                    std::cout << "amount connected clients now: " << connected.size() << std::endl;
                } else {}
                for( auto& it : connected ) {
                    memset(buffer, '\0', sizeof(buffer));
                    if(FD_ISSET(it, &readfds)) {
                        recv(it, buffer, sizeof(buffer), 0);

                        if(strlen(buffer) == 0) {
                            std::cout << "client has disconnected!" << std::endl;
                            it = connected.back();
                            connected.pop_back();
                            std::cout << "amount connected clients now: " << connected.size() << std::endl;
                        } else {
                            std::cout << buffer << std::endl;
                            for(auto& it : connected) {
                                send(it, buffer, sizeof(buffer), 0);
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
