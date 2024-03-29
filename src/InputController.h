//
// Created by gus on 12/27/22.
//

#ifndef UG_INPUTCONTROLLER_H
#define UG_INPUTCONTROLLER_H

#include <unordered_map>
#include <vector>

#include <SDL.h>

enum class Action : uint8_t {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    LOOK_UP,
    LOOK_DOWN,
    LOOK_LEFT,
    LOOK_RIGHT,
    INTERACT,
    EXIT,
    SPEED_UP,
    SPEED_DOWN,
    COUNT
};

class InputController {
private:
    std::unordered_map<SDL_Keycode, Action> mBindings = {
        {SDLK_w,            Action::FORWARD},
        {SDLK_s,            Action::BACKWARD},
        {SDLK_a,            Action::LEFT},
        {SDLK_d,            Action::RIGHT},
        {SDLK_LSHIFT,       Action::DOWN},
        {SDLK_SPACE,        Action::UP},
        {SDLK_k,            Action::LOOK_UP},
        {SDLK_j,            Action::LOOK_DOWN},
        {SDLK_h,            Action::LOOK_LEFT},
        {SDLK_l,            Action::LOOK_RIGHT},
        {SDLK_e,            Action::INTERACT},
        {SDLK_ESCAPE,       Action::EXIT},
        {SDLK_RIGHTBRACKET, Action::SPEED_UP},
        {SDLK_LEFTBRACKET,  Action::SPEED_DOWN},
    };

    std::vector<bool> mPressed;
    std::vector<bool> mHolded;

    float mMouseX = 0;
    float mMouseY = 0;

public:
    explicit InputController();
    void keyDown(SDL_Keycode key);
    void keyUp(SDL_Keycode key);
    void reload();

    bool isActive(Action a);
    bool isPressed(Action a);
    bool isHolded(Action a);

    float getMouseDeltaX();
    float getMouseDeltaY();

    void processEvent(SDL_Event& event);
};

#endif // UG_INPUTCONTROLLER_H
