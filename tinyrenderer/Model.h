//
// Created by vovac on 15.06.2026.
//

#ifndef TINYRENDERER_MODEL_H
#define TINYRENDERER_MODEL_H
#include <filesystem>
#include <vector>

#include "geometry.h"


class Model {
    std::vector<vec3> vertices = {};
    std::vector<std::vector<int>> faces = {};

public:
    Model(const std::filesystem::path& filename);
    int vertices_size();
    int faces_size();
    vec3 get_vector(int index);
    std::vector<int> get_face(int index);
};


#endif //TINYRENDERER_MODEL_H