//
// Created by gus on 12/24/22.
//

#include "Camera.h"

#include <SDL.h>

void Camera::setPosition(const glm::vec3 &position) {
    mPosition = position;
    recalculateViewMatrix();
}

void Camera::setAspectRatio(const float aspectRatio) {
    mAspectRatio = aspectRatio;
    recalculateProjectionMatrix();
}

void Camera::processEvent(SDL_Event event) {
    if (event.type == SDL_WINDOWEVENT) {
        switch (event.window.event) {
            case SDL_WINDOWEVENT_RESIZED:
                float width = event.window.data1;
                float height= event.window.data2;
                setAspectRatio(width / height);
       }
    }
}

Camera::Camera(float aspectRatio) : mAspectRatio(aspectRatio) {
}
