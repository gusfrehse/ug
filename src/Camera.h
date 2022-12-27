//
// Created by gus on 12/24/22.
//

#ifndef UG_CAMERA_H
#define UG_CAMERA_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <SDL.h>

class Camera {
protected:
    glm::vec3 mPosition = glm::vec3(0.0f);
    glm::mat4 mViewMatrix = glm::mat4(1.0f);
    glm::mat4 mProjectionMatrix = glm::mat4(1.0f);

    float mAspectRatio = 1.0f;

    virtual void recalculateViewMatrix() = 0;
    virtual void recalculateProjectionMatrix() = 0;

public:
    explicit Camera(float aspectRatio);

    void setPosition(const glm::vec3& position);
    void setAspectRatio(float aspectRatio);

    const glm::vec3& getPosition() const { return mPosition; }
    const glm::mat4& getViewMatrix() const { return mViewMatrix; }
    const glm::mat4& getProjectionMatrix() const { return mProjectionMatrix; }
    glm::mat4 getProjectionViewMatrix() const { return mProjectionMatrix * mViewMatrix; }

    float getAspectRatio() const { return mAspectRatio; }

    void processEvent(SDL_Event event);
};


#endif //UG_CAMERA_H
