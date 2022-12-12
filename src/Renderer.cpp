//
// Created by gus on 12/11/22.
//

#include "Renderer.h"

#include <cstdio>

#include <SDL.h>
#include "GL/glew.h"
#include "glm/vec4.hpp"

Renderer::Renderer() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

    mWindow = SDL_CreateWindow("TEMP TITLE", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 360, SDL_WINDOW_OPENGL);

    if (!mWindow) {
        std::fprintf(stderr, "[-] ERROR : Couldn't create window: %s\n", SDL_GetError());
        exit(-1);
    }

    mContext = SDL_GL_CreateContext(mWindow);

    GLenum err = glewInit();

    if (GLEW_OK != err) {
        std::fprintf(stderr, "[-] ERROR : Couldn't initialize GLEW: %s\n", glewGetErrorString(err));
        exit(-1);
    }
}

Renderer::~Renderer() {
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void Renderer::drawRenderable(Renderable *renderable) {
}

void Renderer::clearColor(glm::vec4 color) {
    glClearColor(color.x, color.y, color.z, color.w);
}

void Renderer::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
