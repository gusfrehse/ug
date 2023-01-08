//
// Created by gus on 12/11/22.
//

#include "Material.h"

#include <vector>
#include <fstream>
#include <optional>
#include <string>

#include <GL/glew.h>

const std::string shaderDirectoryPrefix("assets/shaders/");

static GLint defaultProgram = 0;
static bool defaultProgramGenerated = false;

static std::optional<std::vector<char>> readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    if (file.fail())
        return std::nullopt;

    std::streamsize size = file.tellg();

    file.seekg(0);

    std::vector<char> buffer(size);

    file.read(buffer.data(), size);

    file.close();

    return buffer;
}

static GLuint createShader(const std::string& shaderPath, GLenum type) {
    auto source = readFile(shaderPath);

    if (!source) {
        std::fprintf(stderr, "[-] ERROR : Couldn't open shader source '%s'\n", shaderPath.c_str());
    }

    // Needed because glShaderSource() expects a null terminated string.
    source->push_back('\0');

    auto shader = glCreateShader(type);

    const GLchar *const sourcePtr = source->data();

    glShaderSource(shader, 1, &sourcePtr, nullptr);

    glCompileShader(shader);

    GLint successful;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &successful);

    if (successful == GL_FALSE) {
        std::fprintf(stderr, "[-] ERROR : Shader '%s' compilation error:\n", shaderPath.c_str());

        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        std::vector<char> infoLog(length + 1);

        glGetShaderInfoLog(shader, length + 1, nullptr, infoLog.data());

        std::fprintf(stderr, "%s", infoLog.data());

        std::exit(1);
    }

    return shader;
}

GLint createProgram(const char *vertexShaderPath, const char *fragmentShaderPath) {
    auto vertexShader = createShader(shaderDirectoryPrefix + vertexShaderPath, GL_VERTEX_SHADER);
    auto fragmentShader = createShader(shaderDirectoryPrefix + fragmentShaderPath, GL_FRAGMENT_SHADER);

    auto program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    GLint successful;
    glGetProgramiv(program, GL_LINK_STATUS, &successful);

    if (successful == GL_FALSE) {
        std::fprintf(stderr, "[-] ERROR : Program linking error:\n");

        GLint length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

        std::vector<char> infoLog(length + 1);

        glGetProgramInfoLog(program, length + 1, nullptr, infoLog.data());

        std::fprintf(stderr, "%s", infoLog.data());

        std::exit(1);
    }

    return program;
}

Material::Material() {
   if (!defaultProgramGenerated) {
       defaultProgram = createProgram("default_vertex.glsl", "default_fragment.glsl");
       defaultProgramGenerated = true;
   }

   mShaderProgram = defaultProgram;
   glUseProgram(mShaderProgram);
}

Material::Material(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) : Material() {
    mShaderProgram = createProgram(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
}

GLint Material::getViewMatrixLocation() {
    if (!mViewMatrixLocation)
        mViewMatrixLocation = glGetUniformLocation(getShaderProgram(), "uView");

    return *mViewMatrixLocation;
}

GLint Material::getProjectionMatrixLocation() {
    if (!mProjectionMatrixLocation)
        mProjectionMatrixLocation = glGetUniformLocation(getShaderProgram(), "uProjection");

    return *mProjectionMatrixLocation;
}

GLint Material::getModelMatrixLocation() {
    if (!mModelMatrixLocation)
        mModelMatrixLocation = glGetUniformLocation(getShaderProgram(), "uModel");

    return *mModelMatrixLocation;
}

void Material::updateUniforms() {
}

FlatColorMaterial::FlatColorMaterial(glm::vec4 col)
    : Material("flat_vertex.glsl", "flat_fragment.glsl") {

    mUniformLocation = glGetUniformLocation(this->getShaderProgram(), "uColor");

    if (mUniformLocation == -1) {
        std::fprintf(stderr, "[-] ERROR: Couldn't get uniform location in FlatColorMaterial!\n");
    }

    updateColor(col);
}

void FlatColorMaterial::updateColor(glm::vec4 color) {
    mColor = color;
    updateUniforms();
}

void FlatColorMaterial::updateUniforms() {
    glUniform4f(mUniformLocation, mColor.x, mColor.y, mColor.z, mColor.w);
}

ShadedColorMaterial::ShadedColorMaterial(glm::vec4 color, glm::vec4 lightPos)
    : Material("shaded_color_vertex.glsl", "shaded_color_fragment.glsl") {

    mColorUniformLocation = glGetUniformLocation(this->getShaderProgram(), "uColor");
    mLightPosUniformLocation = glGetUniformLocation(this->getShaderProgram(), "uLightPos");

    if (mColorUniformLocation == -1) {
        std::fprintf(stderr, "[-] ERROR: Couldn't get color uniform location in ShadedColorMaterial!\n");
    }

    if (mLightPosUniformLocation == -1) {
        std::fprintf(stderr, "[-] ERROR: Couldn't get light pos uniform location in ShadedColorMaterial!\n");
    }

    mColor = color;
    mLightPos = lightPos;
    updateUniforms();
}

void ShadedColorMaterial::updateColor(glm::vec4 color) {
    mColor = color;
    updateUniforms();
}

void ShadedColorMaterial::updateLightPos(glm::vec4 lightPos) {
    mLightPos = lightPos;
    updateUniforms();
}

void ShadedColorMaterial::updateUniforms() {
    glUniform4f(mColorUniformLocation, mColor.x, mColor.y, mColor.z, mColor.w);
    glUniform4f(mLightPosUniformLocation, mLightPos.x, mLightPos.y, mLightPos.z, mLightPos.w);
}

