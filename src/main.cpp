#include <cstdio>

#include "Renderer.h"
#include "App.h"

int main(int, char**) {
    std::printf("Hello world!\n");

    App app(1280, 720);
    Renderer renderer;

    renderer.clearColor(glm::vec4(0.3, 0.8f, 0.2f, 1.0f));
    renderer.clear();

    app.swapWindow();
    getchar();
}
