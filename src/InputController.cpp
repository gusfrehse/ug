//
// Created by gus on 12/27/22.
//

#include "InputController.h"

#include <cstdio>

InputController::InputController() :
    mPressed(static_cast<uint8_t>(Action::COUNT)),
    mHolded(static_cast<uint8_t>(Action::COUNT)) {}

void InputController::keyDown(SDL_Keycode key) {
    if (mBindings.contains(key)) {
        if (!mHolded[static_cast<int>(mBindings[key])]) {
            mPressed[static_cast<int>(mBindings[key])] = true;
            mHolded[static_cast<int>(mBindings[key])] = true;
        }
    }
}

void InputController::keyUp(SDL_Keycode key) {
    if (mBindings.contains(key)) {
        mHolded[static_cast<int>(mBindings[key])] = false;
    }
}

void InputController::reload() {
    for (int i = 0; i < static_cast<int>(Action::COUNT); i++) {
        mPressed[i] = false;
    }

    mMouseX = 0.0f;
    mMouseY = 0.0f;
}

/// You probably don't really want this function.
bool InputController::isActive(Action a) {
    return mPressed[static_cast<int>(a)] || mHolded[static_cast<int>(a)];
}

/// Returns true if the action was entered this frame (after the last reload()).
bool InputController::isPressed(Action a) {
    return mPressed[static_cast<int>(a)];
}

/// Returns true if the action is currently being entered (may miss the input if the key was pressed too late.
bool InputController::isHolded(Action a) {
    return mHolded[static_cast<int>(a)];
}

float InputController::getMouseDeltaX() {
    return mMouseX;
}

float InputController::getMouseDeltaY() {
    return mMouseY;
}

void InputController::processEvent(SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        keyDown(event.key.keysym.sym);
    }

    if (event.type == SDL_KEYUP) {
        keyUp(event.key.keysym.sym);
    }

    if (event.type == SDL_MOUSEMOTION) {
        mMouseX += event.motion.xrel;
        mMouseY += event.motion.yrel;
    }
}

