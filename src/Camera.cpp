//
// Created by gus on 12/24/22.
//

#include "Camera.h"

void Camera::setPosition(const glm::vec3 &position) {
    mPosition = position;
    recalculateViewMatrix();
}

void Camera::setAspectRatio(const float aspectRatio) {
    mAspectRatio = aspectRatio;
    recalculateProjectionMatrix();
}

Camera::Camera(float aspectRatio) : mAspectRatio(aspectRatio) {
}
