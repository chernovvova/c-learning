cmake_minimum_required(VERSION 4.1.2)

project(chip)

include(FetchContent)
FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.4.4
)
FetchContent_MakeAvailable(SDL3)

set(CMAKE_CXX_STANDARD 17)

add_executable(chip main.cpp chip.cpp)

target_link_libraries(chip PRIVATE SDL3::SDL3)
add_custom_command(
        TARGET chip POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3> $<TARGET_FILE_DIR:chip>
)
