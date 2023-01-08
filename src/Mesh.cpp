//
// Created by gus on 12/11/22.
//

#include "Mesh.h"

#include <cstdio>
#include <string>
#include <cassert>

#include <glm/vec4.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
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

    mVertices.resize(2 * numVertices);
    mNormals.resize(2 * numVertices);
    mUvs.resize(2 * numVertices);

    for (int i = 0; i < numVertices; i++) {
        glm::vec4 v = mVertices[i];
        v.w = 0.0f;
        mVertices[numVertices + i] = v;
        mNormals[numVertices + i] = mNormals[i];
        mUvs[numVertices + i] = mUvs[i];
    }

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
     const std::vector<glm::vec2> &uvs, const std::vector<unsigned int> indices)
: Mesh(vertices, normals, uvs)  {
    mEbo = createBufferObject(indices);

    mIndices = indices;

    glVertexArrayElementBuffer(mVao, mEbo);

    numVertices = indices.size();
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


    std::vector<glm::vec4> vertices(result.attributes.positions.size() / 3);
    std::vector<glm::vec3> normals(result.attributes.positions.size() / 3);
    std::vector<glm::vec2> uvs(result.attributes.positions.size() / 3);
    std::vector<unsigned int> indices;

    std::printf("positions in order (index | pos):\n");
    for (int i = 0; i < result.attributes.positions.size() / 3; i++) {
        std::printf("  %d | % 2.4f % 2.4f % 2.4f\n", i, result.attributes.positions[3 * i],
                                                        result.attributes.positions[3 * i + 1],
                                                        result.attributes.positions[3 * i + 2]);

        vertices[i] = glm::vec4(result.attributes.positions[3 * i + 0],
                                result.attributes.positions[3 * i + 1],
                                result.attributes.positions[3 * i + 2],
                                1.0f);
    }

    std::printf("num vertices: %d\n", vertices.size());

    for (const auto& shape : result.shapes) {
        std::printf("shape named \"%s\"\n", shape.name.c_str());
        for (const auto& index : shape.mesh.indices) {
            //std::printf("  position: %5d normal: %5d uv: %5d\n", index.position_index, index.normal_index, index.texcoord_index); 

            indices.push_back(index.position_index);

            if (index.normal_index != -1) {
                normals[index.position_index] = glm::vec3(result.attributes.normals[3 * index.normal_index + 0],
                                                          result.attributes.normals[3 * index.normal_index + 1],
                                                          result.attributes.normals[3 * index.normal_index + 2]);
            } else {
		normals[index.position_index] = glm::vec3(0.0f);
            }

            if (index.texcoord_index != -1) {
                uvs[index.position_index] = glm::vec2(result.attributes.texcoords[2 * index.texcoord_index + 0],
                                                      result.attributes.texcoords[2 * index.texcoord_index + 1]);
            } else {
                uvs[index.position_index] = glm::vec2(0.0f);
            }
        }
    }

    // compressing vectors
    std::vector<unsigned int> newIndices;
    std::vector<glm::vec4> newVertices;
    std::vector<glm::vec3> newNormals;
    std::vector<glm::vec2> newUvs;

    std::unordered_map<glm::vec4, int> vertIndex;
    int redundantVertices = 0;

    for (int i = 0, currentIndex = 0; i < vertices.size(); i++) {
        if (!vertIndex.contains(vertices[i])) {
            newVertices.push_back(vertices[i]);
            newNormals.push_back(normals[i]);
            newUvs.push_back(uvs[i]);

            vertIndex[vertices[i]] = currentIndex++;
        } else
            redundantVertices++;
    }

    std::printf("removed %d redundant vertices\n", redundantVertices);

    for (int i = 0; i < indices.size(); i++) {
        newIndices.push_back(vertIndex[vertices[indices[i]]]);
    }

    std::printf("final has %d vertices and %d indices\n", newVertices.size(), newIndices.size());

    Mesh m(newVertices, newNormals, newUvs, newIndices);
    return m;
}

GLuint Mesh::getVAO() const {
    return mVao;
}

bool Mesh::isIndexed() const {
    return mIndexed;
}
