//
// Created by gus on 12/11/22.
//

#include "Mesh.h"

Mesh::Mesh(std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<glm::vec2> uvs) {

}

static GLuint createVAO() {
    GLuint vao;
    glCreateVertexArrays(1, &vao);

    return vao;
}

GLuint Mesh::getVAO() {
    if (!mVao) {
        mVao = createVAO();
    }

    return *mVao;
}
