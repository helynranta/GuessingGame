#ifndef _LOBBY_HPP
#define _LOBBY_HPP

#include "Engine.hpp"
// http://stackoverflow.com/questions/236129/split-a-string-in-c
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);

class Lobby : public Scene {
private:
    /* private data */
    unsigned int msgs = 0;
    int id = -1;
protected:
    /* protected data */
public:
    /* public data */
    inline Lobby () {}
    inline virtual ~Lobby () {}
    inline void awake() override {
        gui->addText("header", new GUIText(Window::getRenderer(), R::getFont("res/fonts/OpenSans.ttf")));
        gui->addInput("input", new GUIInput(Window::getRenderer(), R::getFont("res/fonts/OpenSans.ttf")));

        gui->getText("header")->setText("CHAT SERVER")->setX(Camera::getWidth()/2);
        gui->getText("header")->setAlign(TEXT_ALIGN::CENTER);

        gui->getInput("input")->show();
        gui->getInput("input")->setMaxLength(42);
        gui->getInput("input")->setY(Camera::getHeight())->setX(0)->setAlign(TEXT_ALIGN::OVER);
        gui->getInput("input")->setBGwidth(Camera::getWidth()*2);
    }

    inline void update(float dt) override {
        std::string msg = InetConnection::update();
        if(msg.length() > 0) {
            std::vector<std::string> msgv = split(msg, ':');
            std::cout << msg << std::endl;
            if(msgv[0] == "clmsg")
            {
                msgs++;
                gui->addText("msg"+std::to_string(msgs), new GUIText(Window::getRenderer(), R::getFont("res/fonts/OpenSans.ttf")));
                gui->getText("msg"+std::to_string(msgs))->setText("User"+msgv[1]+": "+msgv[2]);
                int x = 64;
                for(unsigned int i = msgs; i > 0; i--) {
                    gui->getText("msg"+std::to_string(i))->setY(Camera::getHeight()-x);
                    x += 24;
                }
            } else if(msgv[0] == "newid") {
                msgs++;
                id = atoi(msgv[1].c_str());
                gui->addText("msg"+std::to_string(msgs), new GUIText(Window::getRenderer(), R::getFont("res/fonts/OpenSans.ttf")));
                gui->getText("msg"+std::to_string(msgs))->setText("You have been given a username: User"+msgv[1])->setColor({200,50,50, 255});
                int x = 64;
                for(unsigned int i = msgs; i > 0; i--) {
                    gui->getText("msg"+std::to_string(i))->setY(Camera::getHeight()-x);
                    x += 24;
                }
            } else if(msgv[0] == "srvmsg") {
                msgs++;
                gui->addText("msg"+std::to_string(msgs), new GUIText(Window::getRenderer(), R::getFont("res/fonts/OpenSans.ttf")));
                gui->getText("msg"+std::to_string(msgs))->setText(msgv[1])->setColor({200,50,50, 255});
                int x = 64;
                for(unsigned int i = msgs; i > 0; i--) {
                    gui->getText("msg"+std::to_string(i))->setY(Camera::getHeight()-x);
                    x += 24;
                }
            } else {
                std::cerr << "undefined message recieved!" << std::endl;
                std::cout << msg << std::endl;
            }
        }
        if(Input::isKeyPressed(SDLK_RETURN) && gui->getInput("input")->getText().length() > 1) {
            InetConnection::sendTCP(gui->getInput("input")->getText());
            gui->getInput("input")->setText("");
        } else if(Input::isKeyPressed(SDLK_RETURN)) {
            if(atoi(gui->getInput("input")->getText().c_str()) != 0) {
                InetConnection::sendUDP(gui->getInput("input")->getText()+std::to_string(id));
                gui->getInput("input")->setText("");
            }
        }
    }

    inline void end() override {}

    inline void draw() override {

    }
};

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}
std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

#endif
