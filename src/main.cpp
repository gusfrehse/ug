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
    } 

    if (input.isHolded(Action::BACKWARD)) {
        camera.moveFoward(-speed);
    } 

    if (input.isHolded(Action::RIGHT)) {
        camera.moveRight(speed);
    } 

    if (input.isHolded(Action::LEFT)) {
        camera.moveRight(-speed);
    } 

    if (input.isHolded(Action::UP)) {
        camera.moveUp(speed);
    } 

    if (input.isHolded(Action::DOWN)) {
        camera.moveUp(-speed);
    }

    float sensitivity = 0.1f;

    if (input.isHolded(Action::LOOK_UP)) {
        camera.lookUp(sensitivity);
    }

    if (input.isHolded(Action::LOOK_DOWN)) {
        camera.lookUp(-sensitivity);
    }

    if (input.isHolded(Action::LOOK_RIGHT)) {
        camera.lookRight(sensitivity);
    }

    if (input.isHolded(Action::LOOK_LEFT)) {
        camera.lookRight(-sensitivity);
    }
}

Mesh calculateShadowVolume(glm::vec3 lightPos, const Mesh& input) {
    // TODO: currently we don't use a model matrix hehe, but will use sometime!
    // lightPos = glm::inverse(modelMatrix) * lightPos;

    if (input.isIndexed()) {
        printf("Volumes for indexed meshes are not yet implemented\n");
        exit(1);
    }

    // maybe too slow, perhaps just using a vec<bool> is easier...
    std::unordered_set<std::pair<glm::vec3, glm::vec3>> contourEdges;

    auto updateContourEdge = [&contourEdges](auto& edge) {
        auto edgeIt = contourEdges.find(edge);
        if (edgeIt != contourEdges.end()) {
            contourEdges.erase(edgeIt);
        } else {
            contourEdges.insert(edge);
        }
    };

    auto& vertices = input.getVertices();
    auto& normals = input.getNormals();

    for (int i = 0; i < vertices.size() / 3; i++) {
        // for each triangle...

        glm::vec3 averagePos = (vertices[i] + vertices[i + 1] + vertices[i + 2]) / 3.0f;

        // maybe bogus (they should be all the same?)
        glm::vec3 averageNormal = (normals[i] + normals[i + 1] + normals[i + 2]) / 3.0f;

        glm::vec3 lightDir = averagePos - lightPos;

        if (glm::dot(lightDir, averageNormal) >= 0) {
            updateContourEdge(make_pair(vertices[i + 0], vertices[i + 1]));
            updateContourEdge(make_pair(vertices[i + 1], vertices[i + 2]));
            updateContourEdge(make_pair(vertices[i + 2], vertices[i + 0]));
        }
    }

    // TODO: change vertices to become vec4, because we need points at infinity, also make it easier to make this mesh, without too much copying.
    Mesh out = Mesh(std::vector(contourEdges.begin(), );
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

    camera.setPosition(glm::vec3(0.0f, 0.0f, 2.0f));

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
