//
// Created by gus on 12/11/22.
//

#ifndef UG_RENDERER_H
#define UG_RENDERER_H

#include "Renderable.h"

#include "Camera.h"

#include <glm/vec4.hpp>


class Renderer {
private:
    Camera *mCamera;

public:
    explicit Renderer(Camera *camera);
    Renderer(const Renderer& renderer) = delete;
    ~Renderer();

    void drawRenderable(Renderable *renderable, int numVertices, const unsigned int* indices);
    void drawRenderable(Renderable *renderable);
    void clear();
    void clearColor(glm::vec4 color);
    Camera* getCamera() { return mCamera; }
};

#endif //UG_RENDERER_H
