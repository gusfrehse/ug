//
// Created by gus on 12/11/22.
//

#ifndef UG_MESH_H
#define UG_MESH_H

#include <vector>
#include <optional>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <GL/glew.h>

class Mesh {
private:
    std::vector<glm::vec3> mVertices;
    std::vector<glm::vec3> mNormals;
    std::vector<glm::vec2> mUvs;

    GLuint mVao;
    GLuint mPositionVbo;
    GLuint mNormalVbo;
    GLuint mUvVbo;

public:
    int numVertices;

    bool mIndexed = false;

public:
    Mesh(std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<glm::vec2> uvs);
    GLuint getVAO();
    bool isIndexed();
};


#endif //UG_MESH_H
