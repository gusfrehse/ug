//
// Created by gus on 12/24/22.
//

#include "PerspectiveCamera.h"

#include <glm/gtc/matrix_transform.hpp>

void PerspectiveCamera::recalculateProjectionMatrix() {
    mProjectionMatrix = glm::perspective(mFov, mAspectRatio, mNearPlane, mFarPlane);
}

void PerspectiveCamera::recalculateViewMatrix() {
    mViewMatrix = glm::mat4(1.0);
    mViewMatrix = glm::translate(mViewMatrix, mPosition);
    mViewMatrix = glm::rotate(mViewMatrix, mPitch, glm::vec3(1.0f, 0.0f, 0.0f));
    mViewMatrix = glm::rotate(mViewMatrix, mYaw, glm::vec3(0.0f, 1.0f, 0.0f));
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

PerspectiveCamera::PerspectiveCamera(float aspectRatio) : Camera(aspectRatio) {
    recalculateProjectionMatrix();
}
