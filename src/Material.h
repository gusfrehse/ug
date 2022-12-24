//
// Created by gus on 12/11/22.
//

#ifndef UG_MATERIAL_H
#define UG_MATERIAL_H

#include <string>
#include <optional>

#include <GL/glew.h>

#include <glm/vec4.hpp>

class Material {
private:
    GLuint mShaderProgram = 0;
    std::optional<GLuint> mProjectionMatrixLocation = std::nullopt;
    std::optional<GLuint> mViewMatrixLocation = std::nullopt;
    std::optional<GLuint> mModelMatrixLocation = std::nullopt;

public:
    Material();
    Material(const std::string &vertexShaderPath, const std::string &fragmentShaderPath);

    GLuint getShaderProgram() const { return mShaderProgram; };
    GLuint getProjectionMatrixLocation();
    GLuint getViewMatrixLocation();
    GLuint getModelMatrixLocation();
};

class FlatColorMaterial : public Material {
    glm::vec4 mColor = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
    GLuint mUniformLocation = -1;

public:
    FlatColorMaterial(glm::vec4 col);
    void updateColor(glm::vec4 color);
};

#endif //UG_MATERIAL_H
