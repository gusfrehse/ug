//
// Created by gus on 12/11/22.
//

#ifndef UG_RENDERER_H
#define UG_RENDERER_H

#include "Renderable.h"

#include <glm/vec4.hpp>

class Renderer {
public:
    Renderer();
    Renderer(const Renderer& renderer) = delete;
    ~Renderer();

    void drawRenderable(Renderable *renderable);
    void clear();
    void clearColor(glm::vec4 color);
};

#endif //UG_RENDERER_H
