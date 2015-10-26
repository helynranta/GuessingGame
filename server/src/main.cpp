#include "TCPServer.hpp"

/*
 * CT30A5002 Games and Networking (7 cr)
 * Individual assignment: Chat server
 * REFERENCES:
 * http://www2.mta.ac.il/~eddiea/code-example/Multiplexing/tcp-multiplexing-server.c.html
 */

int main(void) {
    TCPServer* server = new TCPServer();
    server->run();
    server->destroy();
    delete server;
}
