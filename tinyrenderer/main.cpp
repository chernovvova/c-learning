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
    Model model(std::filesystem::path(PROJECT_DIR) / "obj" / "diablo3_pose" / "diablo3_pose.obj");

    for (int i = 0; i < model.faces_size(); i++) {
        auto [ax, ay] = projection(model.get_vector(model.get_face(i)[0]), width, height);
        auto [bx, by] = projection(model.get_vector(model.get_face(i)[1]), width, height);
        auto [cx, cy] = projection(model.get_vector(model.get_face(i)[2]), width, height);

        TGAColor color = {static_cast<uint8_t>(std::rand() % 255), static_cast<uint8_t>(std::rand() % 255), static_cast<uint8_t>(std::rand() % 255), 255};
        triangle(ax, ay, bx, by, cx, cy, framebuffer, color);
    }

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}