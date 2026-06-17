cmake_minimum_required(VERSION 4.1)
project(tinyrenderer)

set(CMAKE_CXX_STANDARD 17)

add_executable(tinyrenderer
        main.cpp
        tgaimage.cpp
        tgaimage.h
        Model.cpp
        Model.h
        geometry.h
)

target_compile_definitions(tinyrenderer PRIVATE PROJECT_DIR="${CMAKE_SOURCE_DIR}")