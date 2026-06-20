#include "tgaimage.h"
#include "geometry.h"
#include "Model.h"
#include <random>

constexpr int width = 1200;
constexpr int height = 900;

const TGAColor white = {255, 255, 255, 255};
const TGAColor green = {0, 255, 0, 255};
const TGAColor red = {255, 0, 0, 255};
const TGAColor blue = {0, 0, 255, 255};
const TGAColor grey = {128, 128, 128, 255};

int main(int argc, char** argv) {
    std::srand(std::time(0));

    TGAImage framebuffer(width, height, TGAImage::RGB);
    TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);
    Model model(std::filesystem::path(PROJECT_DIR) / "obj" / "diablo3_pose" / "diablo3_pose.obj");

    for (int i = 0; i < model.faces_size(); i++) {
        vec3i a_proj = projection(model.get_vector(model.get_face(i)[0]), width, height);
        vec3i b_proj = projection(model.get_vector(model.get_face(i)[1]), width, height);
        vec3i c_proj = projection(model.get_vector(model.get_face(i)[2]), width, height);

        TGAColor color = {
            static_cast<uint8_t>(std::rand() % 255),
            static_cast<uint8_t>(std::rand() % 255),
            static_cast<uint8_t>(std::rand() % 255),
            255
        };
        triangle(a_proj, b_proj, c_proj, framebuffer, zbuffer, color);
    }

    framebuffer.write_tga_file("framebuffer.tga");
    zbuffer.write_tga_file("zbuffer.tga");
    return 0;
}