//
// Created by gus on 12/11/22.
//

#include "Renderer.h"

#include <cstdio>

#include "GL/glew.h"
#include "glm/vec4.hpp"

Renderer::Renderer() {
}

Renderer::~Renderer() {
}

void Renderer::drawRenderable(Renderable *renderable) {
    auto material = renderable->getMaterial();
    auto mesh = renderable->getMesh();

    glBindVertexArray(mesh->getVAO());
    //if (mesh->indexed)
    if (true)
        glDrawElements(GL_TRIANGLES, mesh->numTriangles, GL_UNSIGNED_SHORT, 0);
    else
        glDrawArrays(GL_TRIANGLES, 0, mesh->numTriangles);
}

void Renderer::clearColor(glm::vec4 color) {
    glClearColor(color.x, color.y, color.z, color.w);
}

void Renderer::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
