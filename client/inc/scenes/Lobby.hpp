#ifndef _LOBBY_HPP
#define _LOBBY_HPP

#include "Engine.hpp"

class Lobby : public Scene {
private:
    /* private data */
    unsigned int msgs = 0;
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
            msgs++;
            std::string name = "msg"+std::to_string(msgs);
            gui->addText(name, new GUIText(Window::getRenderer(), R::getFont("res/fonts/OpenSans.ttf")));
            gui->getText(name)->setText(msg);
            int x = 64;
            for(unsigned int i = msgs; i > 0; i--) {
                name = "msg"+std::to_string(i);
                gui->getText(name)->setY(Camera::getHeight()-x);
                x += 24;
            }
        }
        if(Input::isKeyPressed(SDLK_RETURN) && gui->getInput("input")->getText().length() > 0) {
            InetConnection::sendTCP(gui->getInput("input")->getText());
            gui->getInput("input")->setText("");
        }
    }

    inline void end() override {}

    inline void draw() override {

    }
};

#endif
