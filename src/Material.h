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
    GLint mShaderProgram = 0;
    std::optional<GLint> mProjectionMatrixLocation = std::nullopt;
    std::optional<GLint> mViewMatrixLocation = std::nullopt;
    std::optional<GLint> mModelMatrixLocation = std::nullopt;

public:
    Material();
    Material(const std::string &vertexShaderPath, const std::string &fragmentShaderPath);

    GLint getShaderProgram() const { return mShaderProgram; };
    GLint getProjectionMatrixLocation();
    GLint getViewMatrixLocation();
    GLint getModelMatrixLocation();
    virtual void updateUniforms();
};

class FlatColorMaterial : public Material {
    glm::vec4 mColor = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
    GLint mUniformLocation = -1;

public:
    explicit FlatColorMaterial(glm::vec4 col);
    void updateColor(glm::vec4 color);
    void updateUniforms() override;
};

class ShadedColorMaterial : public Material {
    glm::vec4 mColor = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
    glm::vec4 mLightPos = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
    GLint mColorUniformLocation = -1;
    GLint mLightPosUniformLocation = -1;

public:
    explicit ShadedColorMaterial(glm::vec4 color, glm::vec4 lightPos);
    void updateColor(glm::vec4 color);
    void updateLightPos(glm::vec4 lightPos);
    void updateUniforms() override;
};

#endif //UG_MATERIAL_H
