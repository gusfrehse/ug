#include <cstdio>
#include <filesystem>
#include <vector>
#include <unordered_set>
#include <utility>
#include <functional>

#include "Renderer.h"
#include "App.h"
#include "PerspectiveCamera.h"
#include "InputController.h"
#include "DumbPairHash.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>


Mesh createTriangleMesh() {
    return Mesh({
                        glm::vec4(0.0f,-0.5f, 0.0f, 1.0f),
                        glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f),
                        glm::vec4(0.5, 0.5, 0.0f, 1.0f)
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
    float speed = 0.01f;
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
    // lightPos = glm::inverse (modelMatrix) * lightPos;

    if (input.isIndexed()) {
        printf(" Volumes for indexed meshes are not yet implemented\n");
        exit(1);
    }

    // maybe too slow, perhaps just using a vec<bool> is easier...
    std::unordered_set<std::pair<int, int>, DumbPairHash> contourEdges;

    auto updateContourEdge = [&contourEdges](auto edge) {
        auto edgeIt = contourEdges.find(edge);
        if (edgeIt != contourEdges.end()) {
            contourEdges.erase(edgeIt);
        } else {
            contourEdges.insert(edge);
        }
    };

    auto& vertices = input.getVertices();
    auto& normals = input.getNormals();
    auto& uvs = input.getUvs();

    std::printf("size of original vertices %d\n", vertices.size());

    for (int i = 0; i < vertices.size() / 3; i++) {
        // for each triangle...

        glm::vec3 averagePos = (vertices[i] + vertices[i + 1] + vertices[i + 2]) / 3.0f;

        // maybe bogus (they should be all the same?)
        glm::vec3 averageNormal = (normals[i] + normals[i + 1] + normals[i + 2]) / 3.0f;

        glm::vec3 lightDir = averagePos - lightPos;

        if (glm::dot(lightDir, averageNormal) >= 0) {
            updateContourEdge(std::make_pair(i + 0, i + 1));
            updateContourEdge(std::make_pair(i + 1, i + 2));
            updateContourEdge(std::make_pair(i + 2, i + 0));
        }
    }

    // TODO: optimize this probably, done hastly.

    std::printf("%d vertices in the contour\n", contourEdges.size());

    std::vector<glm::vec4> newVertices;
    std::vector<glm::vec3> newNormals(4 * contourEdges.size(), glm::vec3(0.0f));
    std::vector<glm::vec2> newUvs(4 * contourEdges.size(), glm::vec2(0.0f));

    newVertices.reserve(2 * contourEdges.size());

    for (const auto& [a, b] : contourEdges) {
        // we need to add quad with two vertices at infinity?
        glm::vec4 newVertexA = vertices[a] - glm::vec4(lightPos, 0.0f);
        newVertexA.w = 0.0f; // make it at infinity

        glm::vec4 newVertexB = vertices[b] - glm::vec4(lightPos, 0.0f);
        newVertexB.w = 0.0f; // make it at infinity

        newVertices.push_back(vertices[b]); // counter clockwise while viewing from outside the volume.
        newVertices.push_back(vertices[a]);
        newVertices.push_back(newVertexA);

        newVertices.push_back(newVertexA);
        newVertices.push_back(newVertexB);
        newVertices.push_back(vertices[b]);
    }

    // TODO: change vertices to become vec4, because we need points at infinity, also make it easier to make this mesh, without too much copying.
    Mesh out = Mesh(newVertices, newNormals, newUvs);

    return out;
}

int main(int, char**) {
    std::printf("Hello world! Current directory: '%s'\n", std::filesystem::current_path().string().c_str());

    App app(1280, 720);
    PerspectiveCamera camera((float) app.getWidth() / (float) app.getHeight());
    Renderer renderer(&camera);
    InputController input{};

    auto mainMesh = Mesh::fromObjFile("assets/models/teapot.obj");
    auto mainMat = FlatColorMaterial(glm::vec4(0.2f, 0.5f, 0.3, 1.0f));
    auto triangle = Renderable(&mainMesh, &mainMat);
    auto otherMat = FlatColorMaterial(glm::vec4(0.1f, 0.1f, 0.3f, 1.0f));

    glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, -10000.0f);

    Mesh shadowVolumeMesh = calculateShadowVolume(lightPos, mainMesh); 

    auto shadowVolume = Renderable(&shadowVolumeMesh, &otherMat);

    renderer.clearColor(glm::vec4(0.3, 0.8f, 0.2f, 1.0f));

    mainMat.updateColor(glm::vec4(0.1f, 0.2f, 0.6f, 1.0f));

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
        renderer.drawRenderable(&shadowVolume);

        app.swapWindow();

        input.reload();
    }
}
