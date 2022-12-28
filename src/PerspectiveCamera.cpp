//
// Created by gus on 12/24/22.
//

#include "PerspectiveCamera.h"

#include <glm/gtc/matrix_transform.hpp>

#include <cstdio>

void PerspectiveCamera::recalculateProjectionMatrix() {
    mProjectionMatrix = glm::perspective(mFov, mAspectRatio, mNearPlane, mFarPlane);
}

void PerspectiveCamera::recalculateViewMatrix() {
    mViewMatrix = glm::mat4(1.0);
    mViewMatrix = glm::translate(mViewMatrix, mPosition);
    mViewMatrix = glm::rotate(mViewMatrix, mPitch, glm::vec3(1.0f, 0.0f, 0.0f));
    mViewMatrix = glm::rotate(mViewMatrix, mYaw, glm::vec3(0.0f, 1.0f, 0.0f));
    mViewMatrix = glm::inverse(mViewMatrix);
}

void PerspectiveCamera::setFov(float fov) {
    mFov = fov;
    recalculateProjectionMatrix();
}

void PerspectiveCamera::setYaw(float yaw) {
    mYaw = yaw;
    recalculateViewMatrix();
}

void PerspectiveCamera::setPitch(float pitch) {
    mPitch = pitch;
    recalculateViewMatrix();
}

void PerspectiveCamera::moveRight(float amount) {
    glm::vec4 right = getViewMatrix()[0];
    std::printf("right %g %g %g\n", right.x, right.y, right.z);
    setPosition(getPosition() + amount * glm::vec3(right.x, right.y, right.z));
}

void PerspectiveCamera::moveUp(float amount) {
    glm::vec4 up = getViewMatrix()[1];
    std::printf("up %g %g %g\n", up.x, up.y, up.z);
    setPosition(getPosition() + amount * glm::vec3(up.x, up.y, up.z));
}

void PerspectiveCamera::moveFoward(float amount) {
    glm::vec4 forward = getViewMatrix()[2];
    std::printf("forward %g %g %g\n", forward.x, forward.y, forward.z);
    setPosition(getPosition() + amount * glm::vec3(forward.x, forward.y, forward.z));
}

PerspectiveCamera::PerspectiveCamera(float aspectRatio) : Camera(aspectRatio) {
    recalculateProjectionMatrix();
}
