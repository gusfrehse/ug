//
// Created by gus on 12/11/22.
//

#include "Mesh.h"

#include <cstdio>
#include <string>
#include <cassert>

#include <glm/vec4.hpp>
#include <rapidobj/rapidobj.hpp>

static GLuint createVAO() {
    GLuint vao;
    glCreateVertexArrays(1, &vao);

    return vao;
}

template <typename T>
static GLuint createBufferObject(std::vector<T> data) {
    GLuint vbo;
    glCreateBuffers(1, &vbo);

    glNamedBufferData(vbo, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);

    return vbo;
}

Mesh::Mesh(const std::vector<glm::vec4> &vertices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>&
        uvs) : mVertices(vertices), mNormals(normals), mUvs(uvs) {
    numVertices = vertices.size();

    mVao = createVAO();
    glBindVertexArray(mVao);

    mPositionVbo = createBufferObject(vertices);
    mNormalVbo = createBufferObject(normals);
    mUvVbo = createBufferObject(uvs);

    glVertexArrayAttribFormat(mVao, 0, 4, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribFormat(mVao, 1, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribFormat(mVao, 2, 2, GL_FLOAT, GL_FALSE, 0);

    glVertexArrayVertexBuffer(mVao, 0, mPositionVbo, 0, sizeof(glm::vec4));
    glVertexArrayVertexBuffer(mVao, 1, mNormalVbo, 0, sizeof(glm::vec3));
    glVertexArrayVertexBuffer(mVao, 2, mUvVbo, 0, sizeof(glm::vec2));

    glEnableVertexArrayAttrib(mVao, 0);
    glEnableVertexArrayAttrib(mVao, 1);
    glEnableVertexArrayAttrib(mVao, 2);

    glVertexArrayAttribBinding(mVao, 0, 0);
    glVertexArrayAttribBinding(mVao, 1, 1);
    glVertexArrayAttribBinding(mVao, 2, 2);
}

Mesh::Mesh(const std::vector<glm::vec4> &vertices, const std::vector<glm::vec3> &normals,
     const std::vector<glm::vec2> &uvs, const std::vector<int> indices)
: Mesh(vertices, normals, uvs) {
    mEbo = createBufferObject(indices);

    glVertexArrayElementBuffer(mVao, mEbo);

    mIndexed = true;
 }

Mesh Mesh::fromObjFile(const std::string& path) {
    rapidobj::Result result = rapidobj::ParseFile(path.c_str());

    if (result.error) {
        std::printf("[-] ERROR: loading obj \"%s\".", path.c_str());
        exit(1);
    }

    bool success = rapidobj::Triangulate(result);

    if (!success) {
	std::printf("[-] ERROR: triangulating obj \"%s\".", path.c_str());
    }

    std::vector<glm::vec4> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;

    for (const auto& shape : result.shapes) {

        for (const auto& index : shape.mesh.indices) {
            vertices.push_back(glm::vec4(result.attributes.positions[3 * index.position_index + 0],
                                         result.attributes.positions[3 * index.position_index + 1],
                                         result.attributes.positions[3 * index.position_index + 2],
                                         1.0f));

            normals.push_back(glm::vec3(result.attributes.normals[3 * index.normal_index + 0],
                                        result.attributes.normals[3 * index.normal_index + 1],
                                        result.attributes.normals[3 * index.normal_index + 2]));

            uvs.push_back(glm::vec2(result.attributes.texcoords[2 * index.texcoord_index + 0],
                                    result.attributes.texcoords[2 * index.texcoord_index + 1]));
        }
    }

    Mesh m(vertices, normals, uvs);
    return m;
}

GLuint Mesh::getVAO() const {
    return mVao;
}

bool Mesh::isIndexed() const {
    return mIndexed;
}
