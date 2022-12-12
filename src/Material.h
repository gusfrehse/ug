//
// Created by gus on 12/11/22.
//

#ifndef UG_MATERIAL_H
#define UG_MATERIAL_H

#include <string>

#include <GL/glew.h>

class Material {
private:
    GLuint mShaderProgram = 0;

public:
    Material();
    Material(const std::string &vertexShaderPath, const std::string &fragmentShaderPath);

    GLuint getShaderProgram() const { return mShaderProgram; };
};

class PhongShaded : Material {
    float mSpecular = 0.5f;
    float mDiffuse = 0.5f;
};

#endif //UG_MATERIAL_H
