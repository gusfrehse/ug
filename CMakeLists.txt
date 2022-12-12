cmake_minimum_required(VERSION 3.24)

project(ug VERSION 0.1
           DESCRIPTION "Untitled Game"
           LANGUAGES CXX)

find_package(glm REQUIRED)

find_package(SDL2 REQUIRED)

find_package(GLEW REQUIRED)

add_subdirectory(src)
