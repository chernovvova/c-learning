#include "tgaimage.h"
#include "geometry.h"
#include "Model.h"
#include <algorithm>
#include <random>
#include <vector>
#include <limits>
#include "globals.h"

int main(int argc, char** argv) {
    std::srand(std::time(0));

    TGAImage framebuffer(WIDTH, HEIGHT, TGAImage::RGB);
    std::vector<double> zbuffer(WIDTH * HEIGHT, -std::numeric_limits<double>::max());
    Model model(std::filesystem::path(PROJECT_DIR) / "obj" / "diablo3_pose" / "diablo3_pose.obj");

    lookat();
    perspective(norm(eye-center));
    viewport(WIDTH / 16, HEIGHT / 16, WIDTH * 7 / 8, HEIGHT * 7 / 8);

    for (int i = 0; i < model.faces_size(); i++) {
        vec4d clip[3];
        for (int d : {0, 1, 2}) {
            vec3d vector = model.get_vector(model.get_face(i)[d]);
            clip[d] = Perspective * ModelView * vec4d{vector.x, vector.y, vector.z, 1.};
        }
        TGAColor color = {
            static_cast<uint8_t>(std::rand() % 255),
            static_cast<uint8_t>(std::rand() % 255),
            static_cast<uint8_t>(std::rand() % 255),
            255
        };
        rasterize(clip, framebuffer, zbuffer, color);
    }

    framebuffer.write_tga_file("framebuffer.tga");

    TGAImage zbuffer_img(WIDTH, HEIGHT, TGAImage::GRAYSCALE);
    const double bg = -std::numeric_limits<double>::max();
    double zmin = std::numeric_limits<double>::max();
    double zmax = bg;
    for (double z : zbuffer) {
        if (z > bg) { zmin = std::min(zmin, z); zmax = std::max(zmax, z); }
    }
    if (zmax > zmin) {
        for (int i = 0; i < WIDTH * HEIGHT; i++) {
            if (zbuffer[i] == bg) continue;
            uint8_t val = static_cast<uint8_t>((zbuffer[i] - zmin) / (zmax - zmin) * 255);
            zbuffer_img.set(i % WIDTH, i / WIDTH, TGAColor(val));
        }
    }
    zbuffer_img.write_tga_file("zbuffer.tga");
    return 0;
}