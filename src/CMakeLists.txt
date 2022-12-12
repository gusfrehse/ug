add_executable(game main.cpp Renderer.cpp Renderer.h Renderable.cpp Renderable.h Mesh.cpp Mesh.h Material.cpp Material.h)

target_link_libraries(game PRIVATE ${SDL2_LIBRARIES}
                           PRIVATE GLEW::GLEW
                           PRIVATE glm)

target_compile_features(game PUBLIC cxx_std_20)
