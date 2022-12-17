//
// Created by gus on 12/11/22.
//

#ifndef UG_MATERIAL_H
#define UG_MATERIAL_H

#include <string>

#include <GL/glew.h>

#include <glm/vec4.hpp>

class Material {
private:
    GLuint mShaderProgram = 0;

public:
    Material();
    Material(const std::string &vertexShaderPath, const std::string &fragmentShaderPath);

    GLuint getShaderProgram() const { return mShaderProgram; };
};

class FlatColorMaterial : public Material {
    glm::vec4 mColor;

public:
    FlatColorMaterial(glm::vec4 col) : Material("flat_vertex.glsl", "flat_fragment.glsl"), mColor(col) {}
};

#endif //UG_MATERIAL_H
