add_executable(game main.cpp Renderer.cpp Renderer.h Renderable.cpp Renderable.h Mesh.cpp Mesh.h Material.cpp Material.h App.cpp App.h)

target_compile_features(game PUBLIC cxx_std_20)

target_link_libraries(game
        PRIVATE SDL2::SDL2
        PRIVATE SDL2::SDL2main
        PRIVATE glm
        PRIVATE OpenGL::GL
        PRIVATE GLEW::GLEW)
