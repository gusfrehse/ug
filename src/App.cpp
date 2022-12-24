//
// Created by gus on 12/12/22.
//

#include "App.h"

#include <cstdio>

#include <SDL.h>
#include "GL/glew.h"

App::App(int width, int height) : mWidth{width}, mHeight{height} {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

#if 1 // TODO: Actually implement in cmake this.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

    mWindow = SDL_CreateWindow("TEMP TITLE", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 360, SDL_WINDOW_OPENGL);

    if (!mWindow) {
        std::fprintf(stderr, "[-] ERROR : Couldn't create window: %s\n", SDL_GetError());
        exit(-1);
    }

    mContext = SDL_GL_CreateContext(mWindow);

    SDL_GL_SetSwapInterval(0);

    GLenum err = glewInit();

    if (GLEW_OK != err) {
        std::fprintf(stderr, "[-] ERROR : Couldn't initialize GLEW: %s\n", glewGetErrorString(err));
        exit(-1);
    }
}

App::~App() {
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void App::swapWindow() {
    SDL_GL_SwapWindow(mWindow);
}

void App::processEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            mShouldQuit = true;
        }
    }
}

bool App::shouldQuit() {
    return mShouldQuit;
}
