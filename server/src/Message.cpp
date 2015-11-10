#include "Message.hpp"

Message::Message(std::string msg, int tcpsocket)
{
    m_type = TYPE::TCP;
    m_tcpsocket = tcpsocket;
    m_message = msg;
}
Message::Message (std::string msg, int udpsocket, struct sockaddr_in info)
{
    m_type = TYPE::UDP;
    m_contact = info;
    m_contact_len = sizeof(m_contact);
    m_udpsocket = udpsocket;
    m_message = msg;
}
Message::~Message(void) {

}
// tcp send
bool Message::send(void) {
    bool success = true;
    char buffer[1024];
    strcpy(buffer, m_message.c_str());
    if(m_type == TYPE::TCP) {
        if(::send(m_tcpsocket, buffer, sizeof(buffer), 0)<0) {
            success = false;
            std::cerr << "failed to send message: " << m_message << std::endl;
        }
    } else if (m_type == TYPE::UDP) {
        if(::sendto(m_udpsocket, buffer, sizeof(buffer), 0,
        reinterpret_cast<struct sockaddr*>(&m_contact), m_contact_len
        )<0) {
            success = false;
            std::cerr << "failed to send message: " << m_message << std::endl;
        }
    } else  {
        std::cerr << "undefined behaviour " << __FILE__ << std::endl;
        success = false;
    }
    m_sendtime = (clock())/CLOCKS_PER_SEC;
    return success;
}
