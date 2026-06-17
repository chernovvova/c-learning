#include <cmath>
#include <cstdlib>
#include <ctime>

#include "Model.h"
#include "tgaimage.h"

constexpr int width = 800;
constexpr int height = 600;

const TGAColor white = {255, 255, 255, 255};
const TGAColor green = {0, 255, 0, 255};
const TGAColor red = {255, 0, 0, 255};
const TGAColor blue = {0, 0, 255, 255};

void line(int ax, int ay, int bx, int by, TGAImage &framebuffer, TGAColor color) {
    bool steep = std::abs(ax-bx) < std::abs(ay-by);
    if (steep) { // if the line is steep, we transpose the image
        std::swap(ax, ay);
        std::swap(bx, by);
    }
    if (ax>bx) { // make it left−to−right
        std::swap(ax, bx);
        std::swap(ay, by);
    }
    int y = ay;
    int ierror = 0;
    for (int x=ax; x<=bx; x++) {
        if (steep) // if transposed, de−transpose
            framebuffer.set(y, x, color);
        else
            framebuffer.set(x, y, color);
        ierror += 2 * std::abs(by-ay);
        y += (by > ay ? 1 : -1) * (ierror > bx - ax);
        ierror -= 2 * (bx-ax)   * (ierror > bx - ax);
    }
}

std::tuple<int, int> projection(const vec3& vector) {
    return {(vector.x + 1.) * width / 2, (vector.y + 1.) * height / 2};
}

int main(int argc, char** argv) {
    std::filesystem::path model_path = std::filesystem::path(PROJECT_DIR) / "obj" / "diablo3_pose" / "diablo3_pose.obj";
    Model model(model_path);

    TGAImage framebuffer(width, height, TGAImage::RGB);

    for (int i = 0; i < model.faces_size(); i++) {
        std::vector<int> face = model.get_face(i);
        auto [ax, ay] = projection(model.get_vector(face[0]));
        auto [bx, by] = projection(model.get_vector(face[1]));
        auto [cx, cy] = projection(model.get_vector(face[2]));
        line(ax, ay, bx, by, framebuffer, green);
        line(bx, by, cx, cy, framebuffer, green);
        line(ax, ay, cx, cy, framebuffer, green);
    }

    for (int i = 0; i < model.vertices_size(); i++) {
        auto [x, y] = projection(model.get_vector(i));
        framebuffer.set(x, y, red);
    }

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}