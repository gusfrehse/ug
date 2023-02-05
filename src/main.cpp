#include <cstdio>
#include <filesystem>
#include <vector>
#include <unordered_set>
#include <utility>
#include <functional>
#include <tuple>

#include "Renderer.h"
#include "App.h"
#include "PerspectiveCamera.h"
#include "InputController.h"
#include "DumbPairHash.h"
#include "DeltaTimer.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>


Mesh createTriangleMesh() {
    return Mesh({
                        glm::vec4( 0.0f,  5.0f, 0.0f, 1.0f),
                        glm::vec4(-5.0f, -5.0f, 0.0f, 1.0f),
                        glm::vec4( 5.0f, -5.0f, 0.0f, 1.0f)
                },
                {
                        glm::vec3(0.0f, 0.0f, -1.0f),
                        glm::vec3(0.0f, 0.0f, -1.0f),
                        glm::vec3(0.0f, 0.0f, -1.0f),
                },
                {
                        glm::vec2(0.5f, 0.0f),
                        glm::vec2(0.0f, 1.0f),
                        glm::vec2(1.0f, 1.0f),
                },
                { 0, 1, 2 }
    );
}

void updateCamera(PerspectiveCamera& camera, InputController& input, float dt) {
    float speed = 0.01f * dt;
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

    float sensitivity = 0.1f * dt;

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

std::tuple<std::vector<unsigned int>, std::vector<unsigned int>, std::vector<unsigned int>>
calculateShadowVolume(glm::vec4 lightPos, const Mesh& input) {
    // TODO: currently we don't use a model matrix hehe, but will use sometime!
    // lightPos = glm::inverse (modelMatrix) * lightPos;

    if (!input.isIndexed()) {
        printf("[-] ERROR: Volumes for non indexed meshes is not yet implemented\n");
        exit(1);
    }
   auto indices = input.getIndices();
   auto normals = input.getNormals();
   auto vertices = input.getVertices();

    std::unordered_set<std::pair<int, int>, DumbPairHash> contourEdges;

    std::printf("size of original indices %zu\n", indices.size());
    std::printf("light pos: %g %g %g %g\n", lightPos.x, lightPos.y, lightPos.z, lightPos.w);

    auto updateContourEdge = [&contourEdges](std::pair<unsigned int, unsigned int> edge) {
        std::printf("examining edge %d %d\n", edge.first, edge.second);
        auto edgeIt = contourEdges.find(edge);
        if (edgeIt != contourEdges.end()) {
            std::printf("    already there, removing\n");
            contourEdges.erase(edgeIt);
        } else {
            std::printf("    not there, inserting\n");
            contourEdges.insert(edge);
        }
    };

    std::vector<unsigned int> frontCap;
    std::vector<unsigned int> backCap;
    std::vector<unsigned int> silhouette;

    // for each face
    for (int i = 0; i < indices.size() / 3; i++) {
        glm::vec3 averageNormal = (normals[indices[3 * i + 0]] +
                                   normals[indices[3 * i + 1]] +
                                   normals[indices[3 * i + 2]]) / 3.0f;

        glm::vec4 averagePos = (vertices[indices[3 * i]] +
                                vertices[indices[3 * i + 1]] +
                                vertices[indices[3 * i + 2]]) / 3.0f;

        glm::vec3 lightDir = glm::vec3(averagePos - lightPos);

        if (glm::dot(lightDir, averageNormal) <= 0) {
            // front cap
            frontCap.push_back(indices[3 * i + 0]);
            frontCap.push_back(indices[3 * i + 1]);
            frontCap.push_back(indices[3 * i + 2]);

            // silhouette detection
            updateContourEdge(std::minmax(indices[3 * i + 0], indices[3 * i + 1]));
            updateContourEdge(std::minmax(indices[3 * i + 1], indices[3 * i + 2]));
            updateContourEdge(std::minmax(indices[3 * i + 2], indices[3 * i + 0]));
        } else {
            // back cap
            backCap.push_back(indices[3 * i + 0] + input.numVertices);
            backCap.push_back(indices[3 * i + 1] + input.numVertices);
            backCap.push_back(indices[3 * i + 2] + input.numVertices);
        }
    }

    for (const auto& [a, b] : contourEdges) {
        silhouette.push_back(a);
        silhouette.push_back(b);
        silhouette.push_back(a + input.numVertices);

        silhouette.push_back(b);
        silhouette.push_back(b + input.numVertices);
        silhouette.push_back(a + input.numVertices);
    }

    return { frontCap, backCap, silhouette };
}

int main(int, char**) {
    std::printf("Hello world! Current directory: '%s'\n", std::filesystem::current_path().string().c_str());

    App app(1280, 720);
    PerspectiveCamera camera((float) app.getWidth() / (float) app.getHeight());
    Renderer renderer(&camera);
    InputController input{};

    glm::vec4 lightPos = glm::vec4(0.0f, 10.0f, 0.0f, 1.0f);

    auto mainMesh = Mesh::fromObjFile("assets/models/torus.obj");
    //auto mainMesh = createTriangleMesh();
    //auto coordinateSystemMesh = Mesh::fromObjFile("assets/models/coordinate_system.obj");
    auto mainMat = ShadedColorMaterial(glm::vec4(0.3, 0.4, 0.5, 1.0), lightPos);

    auto triangle = Renderable(&mainMesh, &mainMat);

    auto [shadowFrontCapIndices, shadowBackCapIndices, shadowEdgeIndices] = calculateShadowVolume(lightPos, mainMesh); 

    std::printf("shadow front cap indices:\n");
    for (unsigned int i : shadowFrontCapIndices) {
        std::printf(" %u,", i);
    }
    std::printf("\n");

    std::printf("shadow back cap indices:\n");
    for (unsigned int i : shadowBackCapIndices) {
        std::printf(" %u,", i);
    }
    std::printf("\n");

    std::printf("shadow edge indices:\n");
    for (unsigned int i : shadowEdgeIndices) {
        std::printf(" %u", i);
    }
    std::printf("\n");

    renderer.clearColor(glm::vec4(0.3, 0.8f, 0.2f, 1.0f));

    mainMat.updateColor(glm::vec4(0.1f, 0.2f, 0.6f, 1.0f));

    camera.setPosition(glm::vec3(0.0f, 0.0f, 2.0f));

    uint8_t state = 1;

    while (!app.shouldQuit()) {
        DT(dt);
        std::printf("\rdelta time: %3.8f\t\t\t", dt.get());

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            app.processEvent(event);
            camera.processEvent(event);
            input.processEvent(event);
        }

        updateCamera(camera, input, dt.get());

        if (input.isPressed(Action::INTERACT)) {
            state <<= 1;
            if (state & 0b10000) state = 1;
            std::printf("\ninteracted %x\n", state);
            if (state & 0b00001)
                std::printf("original\n");
            if (state & 0b00010)
                std::printf("shadow front cap\n");
            if (state & 0b00100)
                std::printf("shadow edge\n");
            if (state & 0b01000)
                std::printf("shadow back cap\n");
        }

        renderer.clear();

        if (state & 0b0001)
            renderer.drawRenderable(&triangle);
        if (state & 0b0010)
            renderer.drawRenderable(&triangle, shadowFrontCapIndices.size(), shadowFrontCapIndices.data());
        if (state & 0b0100)
            renderer.drawRenderable(&triangle, shadowEdgeIndices.size(), shadowEdgeIndices.data());
        if (state & 0b1000)
            renderer.drawRenderable(&triangle, shadowBackCapIndices.size(), shadowBackCapIndices.data());

        app.swapWindow();

        input.reload();
    }

    std::printf("\nbye!\n");
}
