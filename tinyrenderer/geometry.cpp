#include "geometry.h"

#include "tgaimage.h"

void line(vec2i a, vec2i b, TGAImage &framebuffer, const TGAColor color) {
    bool steep = std::abs(a.x-b.x) < std::abs(a.y-b.y);
    if (steep) {
        std::swap(a.x, a.y);
        std::swap(b.x, b.y);
    }
    if (a.x>b.x) { // make it left−to−right
        std::swap(a.x, b.x);
        std::swap(a.y, b.y);
    }
    int y = a.y;
    int ierror = 0;
    for (int x=a.x; x<=b.x; x++) {
        if (steep)
            framebuffer.set(y, x, color);
        else
            framebuffer.set(x, y, color);
        ierror += 2 * std::abs(b.y-a.y);
        y += (b.y > a.y ? 1 : -1) * (ierror > b.x - a.x);
        ierror -= 2 * (b.x-a.x)   * (ierror > b.x - a.x);
    }
}

double triangle_area(vec3i a, vec3i b, vec3i c) {
    return .5 * ((b.y - a.y) * (b.x + a.x) + (c.y - b.y) * (c.x + b.x) + (a.y - c.y) * (a.x + c.x));
}

void triangle(vec3i a, vec3i b, vec3i c, TGAImage& framebuffer,
    TGAImage& zbuffer,
    const TGAColor color
    ) {
    int bounding_box_min_x = std::min(std::min(a.x, b.x), c.x);
    int bounding_box_min_y = std::min(std::min(a.y, b.y), c.y);
    int bounding_box_max_x = std::max(std::max(a.x, b.x), c.x);
    int bounding_box_max_y = std::max(std::max(a.y, b.y), c.y);

    double area = triangle_area(a, b, c);
    if (area < 1.0) return;

#pragma omp parallel for
    for (int x = bounding_box_min_x; x <= bounding_box_max_x; x++) {
        for (int y = bounding_box_min_y; y <= bounding_box_max_y; y++) {
            const double alpha = triangle_area({x, y}, b, c) / area;
            const double beta = triangle_area({x, y}, c, a) / area;
            const double gamma = triangle_area({x, y}, a, b) / area;

            if (alpha < 0 || beta < 0 || gamma < 0) {
                continue;
            }

            auto z = static_cast<unsigned char>(alpha * a.z + beta * b.z + gamma * c.z);
            if (z <= zbuffer.get(x, y)[0]) continue;

            framebuffer.set(x, y, color);
            zbuffer.set(x, y, {z});
        }
    }
}

vec3i projection(const vec3d& vector, const int width, const int height) {
    int x = (vector.x + 1.) * width / 2;
    int y = (vector.y + 1.) * height / 2;
    int z = (vector.z + 1.) * 255. / 2;
    return {x, y, z};
}