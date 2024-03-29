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
    static float coef = 0.1f;
    float speed = coef * dt;

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

    if (input.isPressed(Action::SPEED_UP)) {
        coef *= 1.3f;
    }

    if (input.isPressed(Action::SPEED_DOWN)) {
        coef *= 0.7f;
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

    camera.lookRight(sensitivity * input.getMouseDeltaX());
    camera.lookUp(-sensitivity * input.getMouseDeltaY());
}

std::tuple<std::vector<unsigned int>, std::vector<unsigned int>, std::vector<unsigned int>>
calculateShadowVolume(glm::vec4 lightPos, const Mesh& input) {
    // TODO: currently we don't use a model matrix heheh, but will use sometime!
    // lightPos = glm::inverse (modelMatrix) * lightPos;

    if (!input.isIndexed()) {
        printf("[-] ERROR: Volumes for non indexed meshes is not yet implemented\n");
        exit(1);
    }

    auto indices = input.getIndices();
    auto normals = input.getNormals();
    auto vertices = input.getVertices();

    std::unordered_set<std::pair<int, int>, DumbPairHash> contourEdges;

    auto updateContourEdge = [&contourEdges](std::pair<unsigned int, unsigned int> edge) {
        auto opEdge = std::make_pair(edge.second, edge.first);
        //std::printf("examining edge %d %d\n", edge.first, edge.second);
        auto edgeIt = contourEdges.find(opEdge);
        if (edgeIt != contourEdges.end()) {
            //std::printf("    already there, removing\n");
            contourEdges.erase(edgeIt);
        } else {
            //std::printf("    not there, inserting\n");
            contourEdges.insert(edge);
        }
    };

    std::vector<unsigned int> frontCap;
    std::vector<unsigned int> backCap;
    std::vector<unsigned int> silhouette;

    // for each face
    for (int i = 0; i < indices.size() / 3; i++) {
        glm::vec3 averageNormal = glm::normalize(normals[indices[3 * i + 0]] +
                                   normals[indices[3 * i + 1]] +
                                   normals[indices[3 * i + 2]]);

        glm::vec4 averagePos = (vertices[indices[3 * i]] +
                                vertices[indices[3 * i + 1]] +
                                vertices[indices[3 * i + 2]]) / 3.0f;

        glm::vec3 lightDir = glm::normalize(glm::vec3(averagePos - lightPos));
        
        if (glm::dot(lightDir, averageNormal) >= 0) {
            // front cap
            frontCap.push_back(indices[3 * i + 1]);
            frontCap.push_back(indices[3 * i + 0]);
            frontCap.push_back(indices[3 * i + 2]);

            // back cap
            backCap.push_back(indices[3 * i + 2] + input.numVertices);
            backCap.push_back(indices[3 * i + 0] + input.numVertices);
            backCap.push_back(indices[3 * i + 1] + input.numVertices);

            // silhouette detection
            updateContourEdge(std::make_pair(indices[3 * i + 0], indices[3 * i + 1]));
            updateContourEdge(std::make_pair(indices[3 * i + 1], indices[3 * i + 2]));
            updateContourEdge(std::make_pair(indices[3 * i + 2], indices[3 * i + 0]));
        }
    }

    for (const auto& [a, b] : contourEdges) {
        silhouette.push_back(a);
        silhouette.push_back(b);
        silhouette.push_back(a + input.numVertices);

        silhouette.push_back(a + input.numVertices);
        silhouette.push_back(b);
        silhouette.push_back(b + input.numVertices);
    }

    return { frontCap, backCap, silhouette };
}

void drawScene(auto& renderer, auto& scene) {
    for (auto& rend : scene) {
        renderer.drawRenderable(&rend);
    }
}

void renderShadowed(auto& renderer, auto& scene, auto& shadowMat, auto& triangle, auto& shadowFrontCapIndices, auto& shadowEdgeIndices, auto& shadowBackCapIndices) {
    auto camera = (PerspectiveCamera*) renderer.getCamera();

    glDrawBuffer(GL_NONE); // dont draw in the framebuffer
 
    // draw scene to depth
    {
        glStencilFunc(GL_ALWAYS, 0x00, 0xFF);
        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
        glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_KEEP);
        shadowMat.updateColor(glm::vec4(0.9, 0.2, 0.5, 1.0));
        drawScene(renderer, scene);
    }

    shadowMat.updateColor(glm::vec4(0.8, 0.6, 0.3, 1.0));

    // render shadow volume
    {
        glDisable(GL_CULL_FACE);

        glStencilFunc(GL_ALWAYS, 0x00, 0xFF);
        glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

        glDepthMask(GL_FALSE); // dont write to depth
        //glDepthFunc(GL_LESS); // always pass depth

        glEnable(GL_DEPTH_CLAMP); // clamp infinity to max depth

        shadowMat.updateColor(glm::vec4(0.8, 0.2, 0.2, 1.0));
        camera->setNearPlane(0.001);
        renderer.drawRenderable(&triangle, shadowFrontCapIndices.size(), shadowFrontCapIndices.data());
        camera->setNearPlane(0.0011);
        shadowMat.updateColor(glm::vec4(0.2, 0.8, 0.2, 1.0));
        renderer.drawRenderable(&triangle, shadowEdgeIndices.size(), shadowEdgeIndices.data());
        shadowMat.updateColor(glm::vec4(0.2, 0.2, 0.8, 1.0));
        renderer.drawRenderable(&triangle, shadowBackCapIndices.size(), shadowBackCapIndices.data());

        glDisable(GL_DEPTH_CLAMP);

        //glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glEnable(GL_CULL_FACE);
    }

    glDrawBuffer(GL_BACK);
    glClear(GL_DEPTH_BUFFER_BIT);
    //glDepthFunc(GL_LE);
    // draw non shadowed scene
    {
        glStencilFunc(GL_EQUAL, 0x00, 0xFF);
        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
        glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_KEEP);

        shadowMat.updateColor(glm::vec4(0.9, 0.2, 0.5, 1.0));
        drawScene(renderer, scene);
    }

}

int main(int argc, char** argv) {

    std::printf("Hello world! Current directory: '%s'\n", std::filesystem::current_path().string().c_str());

    App app(1280, 720);
    PerspectiveCamera camera((float) app.getWidth() / (float) app.getHeight());
    Renderer renderer(&camera);
    InputController input{};
    std::vector<Renderable> scene{};
    std::vector<Renderable> phongScene{};

    glm::vec4 lightPos = glm::vec4(-30.0f, 130.0f, 40.0f, 1.0f);

    const char *modelPath = "assets/models/teapot.obj";

    if (argc == 2)
        modelPath = argv[1];

    auto mainMesh = Mesh::fromObjFile(modelPath);
    //auto mainMesh = createTriangleMesh();
    auto planeMesh = Mesh::fromObjFile("assets/models/plane.obj");
    auto shadowMat = ShadedColorMaterial(glm::vec4(0.3, 0.4, 0.5, 1.0), lightPos);
    auto phongMat = ColorPhongMaterial(glm::vec4(1.0, 0.0f, 0.0f, 1.0), lightPos, &camera);

    auto triangle = Renderable(&mainMesh, &shadowMat);
    auto plane = Renderable(&planeMesh, &shadowMat);

    auto phongRend = Renderable(&mainMesh, &phongMat);
    
    scene.push_back(triangle);
    phongScene.push_back(phongRend);
    //scene.push_back(plane);

    auto [shadowFrontCapIndices, shadowBackCapIndices, shadowEdgeIndices] = calculateShadowVolume(lightPos, mainMesh); 

    //std::printf("shadow front cap indices:\n");
    //for (unsigned int i : shadowFrontCapIndices) {
    //    std::printf(" %u,", i);
    //}
    //std::printf("\n");

    //std::printf("shadow back cap indices:\n");
    //for (unsigned int i : shadowBackCapIndices) {
    //    std::printf(" %u,", i);
    //}
    //std::printf("\n");

    //std::printf("shadow edge indices:\n");
    //for (unsigned int i : shadowEdgeIndices) {
    //    std::printf(" %u", i);
    //}
    //std::printf("\n");

    renderer.clearColor(glm::vec4(0.1, 0.1f, 0.1f, 1.0f));

    shadowMat.updateColor(glm::vec4(0.1f, 0.2f, 0.6f, 1.0f));

    camera.setPosition(glm::vec3(0.0f, 0.0f, 2.0f));

    {
        // TODO: rm this
        int stencilBits;
        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_STENCIL, GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &stencilBits);
        std::printf("stencil bits = %d\n", stencilBits);

    }

    uint8_t state = 1;

    while (!app.shouldQuit()) {
        DT(dt);
        //std::printf("\rdelta time: %3.8f\t\t\t", dt.get());

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            app.processEvent(event);
            camera.processEvent(event);
            input.processEvent(event);
        }

        if (input.isActive(Action::EXIT)) {
            app.quit();
        }

        updateCamera(camera, input, dt.get());

        if (input.isPressed(Action::INTERACT)) {
            state <<= 1;

            if (state & 0b1000) state = 1;

            std::printf("\ninteracted %x\n", state);

            if (state & 0b1) {
                glDisable(GL_STENCIL_TEST);
                std::printf("phong shadow\n");
            }

            if (state & 0b10) {
                glEnable(GL_STENCIL_TEST);
                std::printf("final scene\n");
            }

            if (state & 0b100) {
                glDisable(GL_STENCIL_TEST);
                std::printf("shadow volume\n");
            }
        }

        renderer.clear();

        if (state == 0b1) { 
            drawScene(renderer, phongScene);
        } else if (state == 0b10) {
            renderShadowed(renderer, scene, shadowMat, triangle, shadowFrontCapIndices, shadowEdgeIndices, shadowBackCapIndices);
        } else if (state == 0b100) {
            //glDisable(GL_CULL_FACE);
            glDisable(GL_DEPTH_TEST);
            glStencilFunc(GL_ALWAYS, 0x00, 0xFF);
            glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
            glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_KEEP);

            shadowMat.updateColor(glm::vec4(1.0, 0.0, 0.0, 0.2));
            renderer.drawRenderable(&triangle, shadowFrontCapIndices.size(), shadowFrontCapIndices.data());
            shadowMat.updateColor(glm::vec4(0.0, 1.0, 0.0, 0.2));
            renderer.drawRenderable(&triangle, shadowEdgeIndices.size(), shadowEdgeIndices.data());
            shadowMat.updateColor(glm::vec4(0.0, 0.0, 1.0, 0.2));
            renderer.drawRenderable(&triangle, shadowBackCapIndices.size(), shadowBackCapIndices.data());
            glEnable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
        }

        app.swapWindow();

        input.reload();
    }

    std::printf("\nbye!\n");
}
