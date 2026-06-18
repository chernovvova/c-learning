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

void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage& framebuffer, const TGAColor color) {
    if (ay > by) {
        std::swap(ax, bx);
        std::swap(ay, by);
    }
    if (ay > cy) {
        std::swap(ax, cx);
        std::swap(ay, cy);
    }
    if (by > cy) {
        std::swap(bx, cx);
        std::swap(by, cy);
    }

    if (ay != by) {
        for (int y = ay; y <= by; y++) {
            int x1 = ax + (y - ay) * (cx - ax) / (cy - ay);
            int x2 = ax + (y - ay) * (bx - ax) / (by - ay);
            for (int x = std::min(x1, x2); x <= std::max(x1, x2 ); x++) {
                framebuffer.set(x, y, color);
            }
        }
    }
    if (by != cy) {
        for (int y = by; y <= cy; y++) {
            int x1 = ax + (y - ay) * (cx - ax) / (cy - ay);
            int x2 = bx + (y - by) * (cx - bx) / (cy - by);
            for (int x = std::min(x1, x2); x <= std::max(x1, x2 ); x++) {
                framebuffer.set(x, y, color);
            }
        }
    }
}

std::tuple<int, int> projection(const vec3& vector, const int width, const int height) {
    return {(vector.x + 1.) * width / 2, (vector.y + 1.) * height / 2};
}
