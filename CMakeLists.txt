cmake_minimum_required(VERSION 3.24)

project(ug VERSION 0.1
           DESCRIPTION "Untitled Game"
           LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(rapidobj
    GIT_REPOSITORY https://github.com/guybrush77/rapidobj.git
    GIT_TAG        origin/master)

FetchContent_MakeAvailable(rapidobj)

find_package(glm REQUIRED)

find_package(SDL2 REQUIRED)

find_package(OpenGL REQUIRED)

find_package(GLEW REQUIRED)

add_subdirectory(src)
