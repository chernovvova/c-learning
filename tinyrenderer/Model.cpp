//
// Created by vovac on 15.06.2026.
//

#include "Model.h"

#include <fstream>

Model::Model(const std::filesystem::path& filename) {
    std::ifstream model_file(filename);
    if (!model_file.is_open()) {
        throw std::runtime_error("TGAImage::Model: failed to open model file");
    }

    std::string line;
    while (!model_file.eof()) {
        std::getline(model_file, line);
        std::istringstream line_stream(line);
        char trash;
        if (line.compare(0, 2, "v ") == 0) {
            line_stream >> trash;
            vec3d vertex;
            line_stream >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
        }
        else if (line.compare(0, 2, "f ") == 0) {
            line_stream >> trash;
            int vertex_index, trash1, trash2;
            std::vector<int> face = {};
            while (line_stream >> vertex_index >> trash >> trash1 >> trash >> trash2) {
                face.push_back(--vertex_index);
            }
            if (face.size() != 3) {
                throw std::invalid_argument("TGAImage::Model: model should be triangulated");
            }
            faces.push_back(face);
        }
    }
}

int Model::vertices_size() {
    return vertices.size();
}

int Model::faces_size() {
    return faces.size();
}

vec3d Model::get_vector(const int index) {
    return vertices[index];
}

std::vector<int> Model::get_face(const int index) {
    return faces[index];
}