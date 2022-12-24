//
// Created by gus on 12/24/22.
//

#ifndef UG_PERSPECTIVECAMERA_H
#define UG_PERSPECTIVECAMERA_H

#include "Camera.h"

class PerspectiveCamera : public Camera {
private:
    float mFov = 90.0f;
    float mYaw = 0.0f;
    float mPitch = 0.0f;
    float mNearPlane = 0.01f;
    float mFarPlane = 10.0;

    void recalculateProjectionMatrix() override;
    void recalculateViewMatrix() override;

public:
    explicit PerspectiveCamera(float aspectRatio);
    void setFov(float fov);
    void setYaw(float yaw);
    void setPitch(float pitch);
};


#endif //UG_PERSPECTIVECAMERA_H
