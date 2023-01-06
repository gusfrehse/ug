//
// Created by gus on 12/11/22.
//

#include "Renderer.h"

#include "GL/glew.h"
#include "glm/vec4.hpp"
#include "glm/gtc/type_ptr.hpp"

Renderer::Renderer(Camera *camera) : mCamera(camera) {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2);
}

Renderer::~Renderer() = default;

void Renderer::drawRenderable(Renderable *renderable) {
    auto material = renderable->getMaterial();
    auto mesh = renderable->getMesh();

    glBindVertexArray(mesh->getVAO());

    glUseProgram(material->getShaderProgram());

    glm::mat4 model = glm::mat4(1.0f);

    glUniformMatrix4fv(material->getModelMatrixLocation(), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(material->getViewMatrixLocation(), 1, GL_FALSE, glm::value_ptr(mCamera->getViewMatrix()));
    glUniformMatrix4fv(material->getProjectionMatrixLocation(), 1, GL_FALSE, glm::value_ptr(mCamera->getProjectionMatrix()));

    material->updateUniforms();

    if (mesh->isIndexed())
        glDrawElements(GL_TRIANGLES, mesh->numVertices, GL_UNSIGNED_INT, nullptr);
    else
        glDrawArrays(GL_TRIANGLES, 0, mesh->numVertices);
}

void Renderer::clearColor(glm::vec4 color) {
    glClearColor(color.x, color.y, color.z, color.w);
}

void Renderer::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
