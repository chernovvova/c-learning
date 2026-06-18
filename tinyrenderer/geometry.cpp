#include "geometry.h"

#include "tgaimage.h"

void line(int ax, int ay, int bx, int by, TGAImage &framebuffer, const TGAColor color) {
    bool steep = std::abs(ax-bx) < std::abs(ay-by);
    if (steep) {
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
        if (steep)
            framebuffer.set(y, x, color);
        else
            framebuffer.set(x, y, color);
        ierror += 2 * std::abs(by-ay);
        y += (by > ay ? 1 : -1) * (ierror > bx - ax);
        ierror -= 2 * (bx-ax)   * (ierror > bx - ax);
    }
}

double triangle_area(const int ax, const int ay, const int bx, const int by, const int cx, const int cy) {
    return .5 * ((by - ay) * (bx + ax) + (cy - by) * (cx + bx) + (ay - cy) * (ax + cx));
}

void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage& framebuffer, const TGAColor color) {
    int bounding_box_min_x = std::min(std::min(ax, bx), cx);
    int bounding_box_min_y = std::min(std::min(ay, by), cy);
    int bounding_box_max_x = std::max(std::max(ax, bx), cx);
    int bounding_box_max_y = std::max(std::max(ay, by), cy);

    double area = triangle_area(ax, ay, bx, by, cx, cy);

#pragma omp parallel for
    for (int x = bounding_box_min_x; x <= bounding_box_max_x; x++) {
        for (int y = bounding_box_min_y; y <= bounding_box_max_y; y++) {
            const double alpha = triangle_area(x, y, bx, by, cx, cy) / area;
            const double beta = triangle_area(x, y, cx, cy, ax, ay) / area;
            const double gamma = triangle_area(x, y, ax, ay, bx, by) / area;

            if (alpha < 0 || beta < 0 || gamma < 0) {
                continue;
            }

            framebuffer.set(x, y, color);
        }
    }
}

std::tuple<int, int> projection(const vec3& vector, const int width, const int height) {
    return {(vector.x + 1.) * width / 2, (vector.y + 1.) * height / 2};
}
