//
// Created by vovac on 06.06.2026.
//

#ifndef TINYRENDERER_TGAIMAGE_H
#define TINYRENDERER_TGAIMAGE_H
#include <cstdint>
#include <fstream>
#include <vector>
#include <filesystem>

#pragma pack(push, 1)
struct TGAHeader {
    uint8_t  id_length = 0;
    uint8_t  color_map_type = 0;
    uint8_t  image_type = 0;
    uint16_t color_map_origin = 0;
    uint16_t color_map_length = 0;
    uint8_t  color_map_depth = 0;
    uint16_t x_origin = 0;
    uint16_t y_origin = 0;
    uint16_t width = 0;
    uint16_t height = 0;
    uint8_t  bits_per_pixel = 0;
    uint8_t  image_descriptor = 0;
};
#pragma pack(pop)

struct TGAColor {
    uint8_t bgra[4] = {0, 0, 0, 0};
    uint8_t bytes_per_pixel = 4;
    TGAColor() = default;
    TGAColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a, uint8_t bytes_per_pixel)
        : bgra{b, g, r, a}, bytes_per_pixel{bytes_per_pixel} {};
    uint8_t& operator[](const int i) {return bgra[i];};
};

class TGAImage {
    int width = 0;
    int height = 0;
    uint8_t bytes_per_pixel = 0;
    std::vector<std::uint8_t> data = {};

    bool load_lre_data(std::ifstream &in);
    bool unload_rle_data(std::ofstream &out) const;
public:
    enum Format { GRAYSCALE = 1, RGB = 3, RGBA = 4};
    enum ImageType {
        NO_DATA = 0,
        UNCOMPRESSED_COLOR_MAPPED = 1,
        UNCOMPRESSED_TRUE_COLOR = 2,
        UNCOMPRESSED_GRAYSCALE = 3,
        RLE_COLOR_MAPPED = 9,
        RLE_TRUE_COLOR = 10,
        RLE_GRAYSCALE = 11,
    };
    enum Origin {
        TOP_ORIGIN = 0x20,
        RIGHT_ORIGIN = 0x10,
    };

    TGAImage() = default;
    TGAImage(int width, int height, int bytes_per_pixel);

    bool read_tga_file(std::filesystem::path file_path);
    bool write_tga_file(std::filesystem::path file_path);
    void flip_horizontally();
    void flip_vertically();
    TGAColor get(int x, int y) const;
    void set(int x, int y, TGAColor &color);
    int get_width()  const;
    int get_height() const;
};
#endif //TINYRENDERER_TGAIMAGE_H