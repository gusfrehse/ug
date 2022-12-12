//
// Created by gus on 12/12/22.
//

#ifndef UG_APP_H
#define UG_APP_H

#include <GL/glew.h>
#include <SDL.h>

class App {
    int mWidth = 0;
    int mHeight = 0;
    SDL_Window *mWindow = nullptr;
    SDL_GLContext mContext = nullptr;

public:
    App(int width, int height);
    App(const App& app) = delete;
    ~App();

    void swapWindow();
};


#endif //UG_APP_H
