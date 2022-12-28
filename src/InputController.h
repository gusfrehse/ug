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
    COUNT
};

class InputController {
private:
    std::unordered_map<SDL_Keycode, Action> mBindings = {
        {SDLK_w, Action::FORWARD},
        {SDLK_s, Action::BACKWARD},
        {SDLK_a, Action::LEFT},
        {SDLK_d, Action::RIGHT},
    };

    std::vector<bool> mPressed;
    std::vector<bool> mHolded;

public:
    explicit InputController();
    void keyDown(SDL_Keycode key);
    void keyUp(SDL_Keycode key);
    void reload();

    bool isActive(Action a);
    bool isPressed(Action a);
    bool isHolded(Action a);

    void processEvent(SDL_Event& event);
};

#endif // UG_INPUTCONTROLLER_H
