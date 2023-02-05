//
// Created by gus on 12/11/22.
//

#include "Renderer.h"

#include "GL/glew.h"
#include "glm/vec4.hpp"
#include "glm/gtc/type_ptr.hpp"

void debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *param) {
    const char *typeStr = "[i]";
    const char *severetyStr = "nopers";

    if (type == GL_DEBUG_TYPE_ERROR) 
        typeStr = "[-]";

    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
        severetyStr = "NOTIFICATION ...";
    else if (severity == GL_DEBUG_SEVERITY_LOW)
        severetyStr = "LOW !";
    else if (severity == GL_DEBUG_SEVERITY_MEDIUM)
        severetyStr = "MEDIUM !!";
    else if (severity == GL_DEBUG_SEVERITY_HIGH)
        severetyStr = "HIGH !!!";

    std::fprintf(stderr, "%s OPENGL DEBUG: severity = %s id = 0x%x: %s\n",
                 typeStr, severetyStr, id, message);

}

Renderer::Renderer(Camera *camera) : mCamera(camera) {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(debugCallback, 0);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_CLAMP);

    glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2);
}

Renderer::~Renderer() = default;

void Renderer::drawRenderable(Renderable *renderable) {
    const auto& indices = renderable->getMesh()->getIndices();
    drawRenderable(renderable, indices.size(), indices.data());
}

void Renderer::drawRenderable(Renderable *renderable, int numVertices, const unsigned int* indices) {
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
        glDrawElements(GL_TRIANGLES, numVertices, GL_UNSIGNED_INT, indices);
    else
        glDrawArrays(GL_TRIANGLES, 0, numVertices);
}

void Renderer::clearColor(glm::vec4 color) {
    glClearColor(color.x, color.y, color.z, color.w);
}

void Renderer::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
