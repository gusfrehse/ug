//
// Created by gus on 12/11/22.
//

#include "Material.h"

#include <vector>
#include <fstream>
#include <optional>
#include <string>

const std::string shaderDirectoryPrefix("assets/shaders");

static GLuint defaultProgram = 0;
static bool defaultProgramGenerated = false;

static std::optional<std::vector<char>> readFile(const std::string path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();

    std::vector<char> buffer(size);

    if (file.read(buffer.data(), size)) {
        return buffer;
    }

    return std::nullopt;
}

static GLuint createShader(const std::string shaderPath, GLenum type) {
    auto source = readFile(shaderPath);

    if (!source) {
        std::fprintf(stderr, "[-] ERROR : Couldn't open shader source '%s'\n", shaderPath);
    }

    // Needed because glShaderSource() expects a null terminated string.
    source->push_back('\0');

    auto shader = glCreateShader(type);

    glShaderSource(shader, 1, (const GLchar *const *)source->data(), nullptr);

    glCompileShader(shader);

    GLint successful;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &successful);

    if (successful == GL_FALSE) {
        std::fprintf(stderr, "[-] ERROR : Shader compilation error:\n");

        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        std::vector<char> infoLog(length + 1);

        glGetShaderInfoLog(shader, length + 1, nullptr, infoLog.data());

        std::fprintf(stderr, "%s", infoLog.data());

        std::exit(1);
    }

    return shader;
}

GLuint createProgram(const char *vertexShaderPath, const char *fragmentShaderPath) {
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
}

Material::Material(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) : Material() {
    mShaderProgram = createProgram(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
}

