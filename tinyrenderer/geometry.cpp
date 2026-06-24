#include "geometry.h"

#include <algorithm>
#include <vector>

#include "globals.h"
#include "tgaimage.h"

void rasterize(const vec4d clip[3], TGAImage& framebuffer, std::vector<double>& zbuffer, const TGAColor color) {
    vec4d ndc[3]    = { clip[0] / clip[0].w, clip[1] / clip[1].w, clip[2] / clip[2].w };
    vec2d screen[3] = { (Viewport * ndc[0]).xy(), (Viewport * ndc[1]).xy(), (Viewport * ndc[2]).xy() };

    mat<3 ,3, double> ABC = {
        {
            {screen[0].x, screen[0].y, 1.},
            {screen[1].x, screen[1].y, 1.},
            {screen[2].x, screen[2].y, 1.}
        }
    };
    if (det(ABC) <= 0) return;

    auto [bounding_box_min_x, bounding_box_max_x] = std::minmax({screen[0].x, screen[1].x, screen[2].x});
    auto [bounding_box_min_y, bounding_box_max_y] = std::minmax({screen[0].y, screen[1].y, screen[2].y});

#pragma omp parallel for
    for (int x = std::max<int>(bounding_box_min_x, 0); x <= std::min<int>(bounding_box_max_x, framebuffer.get_width() - 1); x++) {
        for (int y = std::max<int>(bounding_box_min_y, 0); y <= std::min<int>(bounding_box_max_y, framebuffer.get_height() - 1); y++) {
            vec3d bc = inverse(ABC.transpose()) * vec3d{static_cast<double>(x), static_cast<double>(y), 1.};
            if (bc.x < 0 || bc.y < 0 || bc.z < 0) {
                continue;
            }
            double z = bc * vec3d{ndc[0].z, ndc[1].z, ndc[2].z};
            int idx = x + y * framebuffer.get_width();
            if (z <= zbuffer[idx]) continue;
            zbuffer[idx] = z;
            framebuffer.set(x, y, color);
        }
    }
}

void viewport(const int x, const int y, const int width, const int height) {
    Viewport = {
        {
            {width / 2., 0, 0, x + width / 2.},
            {0, height / 2., 0, y + height / 2.},
            {0, 0, 1, 0},
            {0, 0, 0, 1}
        }
    };
}

void perspective(const double f) {
    Perspective = {
        {
            {1, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 1, 0},
            {0, 0, -1 / f, 1}
        }
    };
}

void lookat() {
    const vec3d n = normalized(eye - center);
    const vec3d l = normalized(up - center);
    const vec3d m = normalized(cross(up, n));
    ModelView = mat<4, 4, double>{{{m.x, m.y, m.z, 0}, {l.x, l.y, l.z, 0}, {n.x, n.y, n.z, 0}, {0, 0, 0, 1}}}
        * mat<4, 4, double>{{{1, 0, 0, -center.x}, {0, 1, 0, -center.y}, {0, 0, 1, -center.z}, {0, 0, 0, 1}}};
}


