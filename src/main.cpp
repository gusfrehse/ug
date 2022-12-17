#include <cstdio>
#include <filesystem>

#include "Renderer.h"
#include "App.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>


Mesh createTriangleMesh() {
    return Mesh(
            {glm::vec3(-1.0f), glm::vec3(-1.0f), glm::vec3(0.0f)},
            { glm::vec3(1.0f), glm::vec3(-1.0f), glm::vec3(0.0f)},
            { glm::vec2(0.0f),  glm::vec2(1.0f), glm::vec2(0.0f)}
            );
}

int main(int, char**) {
    std::printf("Hello world! Current directory: '%s'\n", std::filesystem::current_path().string().c_str());

    App app(1280, 720);
    Renderer renderer;

    auto triangleMesh = createTriangleMesh();
    auto triangleMat = FlatColorMaterial(glm::vec4(0.2f, 0.5f, 0.3, 1.0f));
    auto triangle = Renderable(&triangleMesh, &triangleMat);

    renderer.clearColor(glm::vec4(0.3, 0.8f, 0.2f, 1.0f));

    renderer.drawRenderable(&triangle);

    renderer.clear();

    app.swapWindow();
    getchar();
}
