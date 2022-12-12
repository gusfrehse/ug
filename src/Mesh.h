//
// Created by gus on 12/11/22.
//

#ifndef UG_MESH_H
#define UG_MESH_H

#include <vector>

#include "glm/vec3.hpp"
#include "glm/vec2.hpp"

class Mesh {
private:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
};


#endif //UG_MESH_H
