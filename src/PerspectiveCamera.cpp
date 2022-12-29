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
    mInverseViewMatrix = glm::mat4(1.0);
    mInverseViewMatrix = glm::translate(mInverseViewMatrix, mPosition);
    mInverseViewMatrix = glm::rotate(mInverseViewMatrix, glm::radians(mYaw), glm::vec3(0.0f, 1.0f, 0.0f));
    mInverseViewMatrix = glm::rotate(mInverseViewMatrix, glm::radians(mPitch), glm::vec3(1.0f, 0.0f, 0.0f));
    mViewMatrix = glm::inverse(mInverseViewMatrix);
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
    glm::vec4 cameraRight = mInverseViewMatrix[0];
    glm::vec3 right = glm::normalize(glm::vec3(cameraRight.x, 0.0f, cameraRight.z));
    setPosition(getPosition() + amount * right);
}

void PerspectiveCamera::moveUp(float amount) {
    // glm::vec4 up = mInverseViewMatrix[1];
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    setPosition(getPosition() + amount * glm::vec3(up.x, up.y, up.z));
}

void PerspectiveCamera::moveFoward(float amount) {
    glm::vec4 cameraForward = -mInverseViewMatrix[2];
    glm::vec3 forward = glm::normalize(glm::vec3(cameraForward.x, 0.0f, cameraForward.z));
    setPosition(getPosition() + amount * forward);
}

void PerspectiveCamera::lookUp(float amount) {
    mPitch += amount / 2.0f;
    float pitch = mPitch + amount;
    setPitch(glm::clamp(pitch, -89.99f, 89.99f));
}

void PerspectiveCamera::lookRight(float amount) {
    float yaw = mYaw - amount;
    if (yaw < -180.0f)
        yaw += 360.0f;
    else if (yaw > 180.0f)
        yaw -= 360.0f;

    setYaw(yaw);
}

PerspectiveCamera::PerspectiveCamera(float aspectRatio) : Camera(aspectRatio) {
    recalculateProjectionMatrix();
}
