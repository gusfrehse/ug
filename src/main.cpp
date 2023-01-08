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
std::tuple<Mesh, Mesh, Mesh> calculateShadowVolume(glm::vec4 lightPos, const Mesh& input) {
    // TODO: currently we don't use a model matrix hehe, but will use sometime!
    // lightPos = glm::inverse (modelMatrix) * lightPos;

    if (!input.isIndexed()) {
        printf("[-] ERROR: Volumes for non indexed meshes are not yet implemented\n");
        exit(1);
    }

    auto vertices = input.getVertices();
    auto normals = input.getNormals();
    auto uvs = input.getUvs();
    auto indices = input.getIndices();

    std::unordered_set<std::pair<int, int>, DumbPairHash> contourEdges;

    std::printf("size of original vertices %d\n", vertices.size());

    auto updateContourEdge = [&contourEdges, &vertices](std::pair<int, int> edge) {
        std::printf("  examining edge <%d, %d> = <(%3.3f, %3.3f, %3.3f), (%3.3f, %3.3f, %3.3f)>\n",
                    edge.first, edge.second,
                    vertices[edge.first].x, vertices[edge.first].y, vertices[edge.first].z,
                    vertices[edge.second].x, vertices[edge.second].y, vertices[edge.second].z);
        auto edgeIt = contourEdges.find(edge);
        if (edgeIt != contourEdges.end()) {
            std::printf("    already there, removing\n");
            contourEdges.erase(edgeIt);
        } else {
            std::printf("    not there, inserting\n");
            contourEdges.insert(edge);
        }
    };

    std::vector<glm::vec4> frontCap;
    std::vector<glm::vec4> backCap;
    std::vector<glm::vec4> silhouette;

    // for each face
    for (int i = 0; i < indices.size() / 3; i++) {
        glm::vec3 averageNormal = (normals[i] + normals[i + 1] + normals[i + 2]) / 3.0f;

        glm::vec4 averagePos = (vertices[i] + vertices[i + 1] + vertices[i + 2]) / 3.0f;

        glm::vec3 lightDir = glm::vec3(averagePos - lightPos);

        if (glm::dot(lightDir, averageNormal) >= 0) {
            // front cap
            frontCap.push_back(vertices[indices[i]]);
            frontCap.push_back(vertices[indices[i + 1]]);
            frontCap.push_back(vertices[indices[i + 2]]);

            // silhouette detection
            updateContourEdge(std::minmax(indices[i + 0], indices[i + 1]));
            updateContourEdge(std::minmax(indices[i + 1], indices[i + 2]));
            updateContourEdge(std::minmax(indices[i + 2], indices[i + 0]));
        } else {
            // back cap
            glm::vec4 a = vertices[indices[i]];
            a.w = 0.0f;
            glm::vec4 b = vertices[indices[i + 1]];
            b.w = 0.0f;
            glm::vec4 c = vertices[indices[i + 2]];
            c.w = 0.0f;

            backCap.push_back(b);
            backCap.push_back(a);
            backCap.push_back(c);
        }
    }

    for (const auto& [a, b] : contourEdges) {
        glm::vec4 va(vertices[a]);
        glm::vec4 va_inf = vertices[a] - lightPos;
        va_inf.w = 0.0f;

        glm::vec4 vb(vertices[b]);
        glm::vec4 vb_inf = vertices[b] - lightPos;
        vb_inf.w = 0.0f;

        silhouette.push_back(va);
        silhouette.push_back(vb);
        silhouette.push_back(va_inf);

        silhouette.push_back(vb);
        silhouette.push_back(vb_inf);
        silhouette.push_back(va_inf);
    }

    normals.resize(frontCap.size());
    uvs.resize(frontCap.size());
    Mesh frontCapMesh(frontCap, normals, uvs);

    normals.resize(backCap.size());
    uvs.resize(backCap.size());
    Mesh backCapMesh(backCap, normals, uvs);

    normals.resize(silhouette.size());
    uvs.resize(silhouette.size());
    Mesh silhouetteMesh(silhouette, normals, uvs);

    return { frontCapMesh, backCapMesh, silhouetteMesh };
}

#if 0

Mesh calculateShadowVolume(glm::vec3 lightPos, const Mesh& input) {
    // TODO: move Renderer::renderRenderable() to Renderable.
    // TODO: currently we don't use a model matrix hehe, but will use sometime!
    // lightPos = glm::inverse (modelMatrix) * lightPos;

    if (input.isIndexed()) {
        printf(" Volumes for indexed meshes are not yet implemented\n");
        exit(1);
    }

    auto& vertices = input.getVertices();
    auto& normals = input.getNormals();
    auto& uvs = input.getUvs();

    std::printf("size of original vertices %d\n", vertices.size());

    // maybe too slow, perhaps just using a vec<bool> is easier...
    std::unordered_set<std::pair<glm::vec4, glm::vec4>, DumbPairHash> contourEdges;

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

    std::printf("%d edges in the contour\n", contourEdges.size());


    std::vector<glm::vec4> newVertices;
    std::vector<glm::vec3> newNormals(4 * contourEdges.size(), glm::vec3(0.0f));
    std::vector<glm::vec2> newUvs(4 * contourEdges.size(), glm::vec2(0.0f));

    newVertices.reserve(2 * contourEdges.size());

    for (const auto& [a, b] : contourEdges) {
        // we need to add quad with two vertices at infinity?
        glm::vec4 newVertexA = a - glm::vec4(lightPos, 0.0f);
        newVertexA.w = 0.0f; // make it at infinity

        glm::vec4 newVertexB = b - glm::vec4(lightPos, 0.0f);
        newVertexB.w = 0.0f; // make it at infinity

        newVertices.push_back(b); // counter clockwise while viewing from outside the volume.
        newVertices.push_back(a);
        newVertices.push_back(newVertexA);

        newVertices.push_back(newVertexA);
        newVertices.push_back(newVertexB);
        newVertices.push_back(b);
    }


    // TODO: change vertices to become vec4, because we need points at infinity, also make it easier to make this mesh, without too much copying.
    Mesh out = Mesh(newVertices, newNormals, newUvs);

    return out;
}

#endif

int main(int, char**) {
    std::printf("Hello world! Current directory: '%s'\n", std::filesystem::current_path().string().c_str());

    App app(1280, 720);
    PerspectiveCamera camera((float) app.getWidth() / (float) app.getHeight());
    Renderer renderer(&camera);
    InputController input{};

    glm::vec4 lightPos = glm::vec4(0.0f, 0.0f, -10.0f, 1.0f);

    auto mainMesh = Mesh::fromObjFile("assets/models/torus.obj");
    auto mainMat = ShadedColorMaterial(glm::vec4(0.2f, 0.5f, 0.3, 1.0f), lightPos);
    auto triangle = Renderable(&mainMesh, &mainMat);

    auto indices = mainMesh.getIndices();

    auto [shadowFrontCapMesh, shadowBackCapMesh, shadowEdgeMesh] = calculateShadowVolume(lightPos, mainMesh); 

    auto shadowFrontCap = Renderable(&shadowFrontCapMesh, &mainMat);
    auto shadowBackCap = Renderable(&shadowBackCapMesh, &mainMat);
    auto shadowEdge = Renderable(&shadowEdgeMesh, &mainMat);

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
            if (state & 0b100000) state = 1;
            std::printf("\ninteracted %x\n", state);
            if (state & 0b000)
                std::printf("nothing\n");
            if (state & 0b001)
                std::printf("shadow back cap\n");
            if (state & 0b010)
                std::printf("shadow edge\n");
            if (state & 0b100)
                std::printf("shadow front cap\n");
            if (state & 0b1000)
                std::printf("original\n");
            if (state & 0b10000)
                std::printf("original at infinity\n");
        }

        renderer.clear();

        if (state & 0b001)
            renderer.drawRenderable(&shadowBackCap);
        if (state & 0b010)
            renderer.drawRenderable(&shadowEdge);
        if (state & 0b100)
            renderer.drawRenderable(&shadowFrontCap);
        if (state & 0b1000)
            renderer.drawRenderable(&triangle);
        if (state & 0b10000)
            renderer.drawRenderable(&triangle, indices.size(), indices.data());

        app.swapWindow();

        input.reload();
    }
}
