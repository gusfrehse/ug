//
// Created by gus on 12/11/22.
//

#include "Mesh.h"
#include <cstdio>

static GLuint createVAO() {
    GLuint vao;
    glCreateVertexArrays(1, &vao);

    return vao;
}

template <typename T>
static GLuint createVBO(std::vector<T> data) {
    GLuint vbo;
    glCreateBuffers(1, &vbo);

    glNamedBufferData(vbo, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);

    return vbo;
}

Mesh::Mesh(std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<glm::vec2> uvs) {

    std::printf("vertices:\n");
    for (const auto& v : vertices) {
        std::printf("\t%g %g %g\n", v.x, v.y, v.z);
    }

    std::printf("normals:\n");
    for (const auto& v : vertices) {
        std::printf("\t%g %g %g\n", v.x, v.y, v.z);
    }

    std::printf("uvs:\n");
    for (const auto &v : vertices) {
        std::printf("\t%g %g %g\n", v.x, v.y, v.z);
    }

    numVertices = vertices.size();

    mVao = createVAO();
    glBindVertexArray(mVao);

    mPositionVbo = createVBO(vertices);
    mNormalVbo = createVBO(normals);
    mUvVbo = createVBO(uvs);

    glVertexArrayAttribFormat(mVao, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribFormat(mVao, 1, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribFormat(mVao, 2, 2, GL_FLOAT, GL_FALSE, 0);

    glVertexArrayVertexBuffer(mVao, 0, mPositionVbo, 0, sizeof(glm::vec3));
    glVertexArrayVertexBuffer(mVao, 1, mNormalVbo, 0, sizeof(glm::vec3));
    glVertexArrayVertexBuffer(mVao, 2, mUvVbo, 0, sizeof(glm::vec2));

    glEnableVertexArrayAttrib(mVao, 0);
    glEnableVertexArrayAttrib(mVao, 1);
    glEnableVertexArrayAttrib(mVao, 2);

    glVertexArrayAttribBinding(mVao, 0, 0);
    glVertexArrayAttribBinding(mVao, 1, 1);
    glVertexArrayAttribBinding(mVao, 2, 2);
}

GLuint Mesh::getVAO() {
    return mVao;
}

bool Mesh::isIndexed() {
    return mIndexed;
}
