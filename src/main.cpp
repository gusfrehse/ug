#include <cstdio>
#include <filesystem>

#include "Renderer.h"
#include "App.h"
#include "PerspectiveCamera.h"
#include "InputController.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>


Mesh createTriangleMesh() {
    return Mesh({
                        glm::vec3(0.0f,-0.5f, 0.0f),
                        glm::vec3(-0.5f, 0.5f, 0.0f),
                        glm::vec3(0.5, 0.5, 0.0f)
                },
                {
                        glm::vec3(0.0f, 0.0f, 1.0f),
                        glm::vec3(0.0f, 0.0f, 1.0f),
                        glm::vec3(0.0f, 0.0f, 1.0f),
                },
                {
                        glm::vec2(0.5f, 0.0f),
                        glm::vec2(0.0f, 1.0f),
                        glm::vec2(1.0f, 1.0f),
                }
    );
}

void updateCamera(PerspectiveCamera& camera, InputController& input) {
    float speed = 0.001f;
    if (input.isHolded(Action::FORWARD)) {
        camera.moveFoward(speed);
    } else if (input.isHolded(Action::BACKWARD)) {
        camera.moveFoward(-speed);
    } else if (input.isHolded(Action::RIGHT)) {
        camera.moveRight(speed);
    } else if (input.isHolded(Action::LEFT)) {
        camera.moveRight(-speed);
    } else if (input.isHolded(Action::UP)) {
        camera.moveUp(speed);
    } if (input.isHolded(Action::DOWN)) {
        camera.moveUp(-speed);
    }
 }

int main(int, char**) {
    std::printf("Hello world! Current directory: '%s'\n", std::filesystem::current_path().string().c_str());

    App app(1280, 720);
    PerspectiveCamera camera((float) app.getWidth() / (float) app.getHeight());
    Renderer renderer(&camera);
    InputController input{};

    auto triangleMesh = createTriangleMesh();
    auto triangleMat = FlatColorMaterial(glm::vec4(0.2f, 0.5f, 0.3, 1.0f));
    auto triangle = Renderable(&triangleMesh, &triangleMat);

    renderer.clearColor(glm::vec4(0.3, 0.8f, 0.2f, 1.0f));

    triangleMat.updateColor(glm::vec4(0.1f, 0.2f, 0.6f, 1.0f));

    camera.setPosition(glm::vec3(0.0f, 0.0f, -2.0f));

    long long count = 0;

    while (!app.shouldQuit()) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            app.processEvent(event);
            camera.processEvent(event);
            input.processEvent(event);
        }

        updateCamera(camera, input);

        renderer.clear();

        renderer.drawRenderable(&triangle);

        app.swapWindow();

	input.reload();
    }
}
