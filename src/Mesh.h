//
// Created by gus on 12/11/22.
//

#ifndef UG_MESH_H
#define UG_MESH_H

#include <vector>
#include <optional>
#include <string>

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <GL/glew.h>

class Mesh {
private:
    std::vector<glm::vec4> mVertices;
    std::vector<glm::vec3> mNormals;
    std::vector<glm::vec2> mUvs;
    std::vector<unsigned int> mIndices;

    GLuint mVao;
    GLuint mPositionVbo;
    GLuint mNormalVbo;
    GLuint mUvVbo;
    GLuint mEbo;

public:
    int numVertices;

    bool mIndexed = false;

public:
    Mesh(const std::vector<glm::vec4> &vertices, const std::vector<glm::vec3> &normals,
         const std::vector<glm::vec2> &uvs);

    Mesh(const std::vector<glm::vec4> &vertices, const std::vector<glm::vec3> &normals,
         const std::vector<glm::vec2> &uvs, const std::vector<unsigned int> indices);

    static Mesh fromObjFile(const std::string& path);

    GLuint getVAO() const;
    const std::vector<glm::vec4>& getVertices() const { return mVertices; }
    const std::vector<glm::vec3>& getNormals() const { return mNormals; }
    const std::vector<glm::vec2>& getUvs() const { return mUvs; }
    const std::vector<unsigned int>& getIndices() const { return mIndices; }
    bool isIndexed() const;
};


#endif //UG_MESH_H
