#include "tgaimage.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <cstring>
#include <vector>

TGAImage::TGAImage(const int width, const int height, const int bytes_per_pixel) {
    this->width = width;
    this->height = height;
    this->bytes_per_pixel = bytes_per_pixel;
    this->data = std::vector<uint8_t>(width * height * bytes_per_pixel, 0);
}

bool TGAImage::read_tga_file(std::filesystem::path file_path) {
    std::ifstream in;
    in.open(file_path, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "TGAImage::read_tga_file: failed to open file" << std::endl;
        return false;
    }

    TGAHeader header;
    in.read(reinterpret_cast<char*>(&header), sizeof(header));
    if (!in.good()) {
        std::cerr << "TGAImage::read_tga_file: failed to read header" << std::endl;
        return false;
    }

    width = header.width;
    height = header.height;
    bytes_per_pixel = header.bits_per_pixel >> 3;

    if (width <= 0 || height <= 0) {
        std::cerr << "TGAImage::read_tga_file: invalid width/height" << std::endl;
        return false;
    }
    if (bytes_per_pixel != GRAYSCALE && bytes_per_pixel != RGB && bytes_per_pixel != RGBA) {
        std::cerr << "TGAImage::read_tga_file: invalid bytes_per_pixel" << std::endl;
        return false;
    }

    const size_t number_of_bytes = width * height * bytes_per_pixel;
    data = std::vector<uint8_t>(number_of_bytes, 0);
    if (header.image_type == UNCOMPRESSED_GRAYSCALE || header.image_type == UNCOMPRESSED_TRUE_COLOR) {
        in.read(reinterpret_cast<char*>(data.data()), number_of_bytes);
        if (!in.good()) {
            std::cerr << "TGAImage::read_tga_file: failed to read data" << std::endl;
            return false;
        }
    }
    else if (header.image_type == RLE_TRUE_COLOR || header.image_type == RLE_GRAYSCALE) {
        if (!load_rle_data(in)) {
            std::cerr << "TGAImage::read_tga_file: failed to read data" << std::endl;
            return false;
        }
    }
    else {
        std::cerr << "TGAImage::read_tga_file: invalid image type: " << header.image_type << std::endl;
        return false;
    }
    if (!(header.image_descriptor & TOP_ORIGIN)) {
        flip_vertically();
    }
    if (header.image_descriptor & RIGHT_ORIGIN) {
        flip_horizontally();
    }
    return true;
}

bool TGAImage::load_rle_data(std::ifstream& in) {
    size_t pixel_count = width * height;
    size_t current_pixel = 0;
    size_t current_byte = 0;
    TGAColor color_buffer;

    do {
        uint8_t chunk_header = 0;
        chunk_header = in.get();

        if (!in.good()) {
            std::cerr << "TGAImage::load_lre_data: failed to read chunk header" << std::endl;
            return false;
        }

        if (chunk_header < 128) {
            chunk_header++;
            for (int i = 0; i < chunk_header; i++) {
                in.read(reinterpret_cast<char*>(color_buffer.bgra), bytes_per_pixel);
                if (!in.good()) {
                    std::cerr << "TGAImage::load_lre_data: failed to read chunk data" << std::endl;
                    return false;
                }
                for (int b = 0; b < bytes_per_pixel; b++) {
                    data[current_byte++] = color_buffer.bgra[b];
                }
                current_pixel++;
                if (current_pixel > pixel_count) {
                    std::cerr << "TGAImage::load_lre_data: too many pixels read" << std::endl;
                    return false;
                }
            }
        }
        else {
            chunk_header -= 127;
            in.read(reinterpret_cast<char*>(color_buffer.bgra), bytes_per_pixel);
            if (!in.good()) {
                std::cerr << "TGAImage::load_lre_data: failed to read chunk data" << std::endl;
                return false;
            }
            for (int i = 0; i < chunk_header; i++) {
                for (int b = 0; b < bytes_per_pixel; b++) {
                    data[current_byte++] = color_buffer.bgra[b];
                }
                current_pixel++;
                if (current_pixel > pixel_count) {
                    std::cerr << "TGAImage::load_lre_data: too many pixels read" << std::endl;
                    return false;
                }
            }
        }
    } while (current_pixel < pixel_count);
    return true;
}

bool TGAImage::write_tga_file(const std::filesystem::path& file_path, bool vertical_flip, bool rle) {
    constexpr uint8_t developer_area_ref[4] = {0, 0, 0, 0};
    constexpr uint8_t extension_area_ref[4] = {0, 0, 0, 0};
    constexpr uint8_t footer[18] = {'T','R','U','E','V','I','S','I','O','N','-','X','F','I','L','E','.','\0'};
    std::ofstream out;
    out.open(file_path, std::ios::binary);

    if (!out.is_open()) {
        std::cerr << "TGAImage::write_tga_file: failed to open file" << std::endl;
        return false;
    }

    TGAHeader header = {};
    header.bits_per_pixel = bytes_per_pixel << 3;
    header.width  = width;
    header.height = height;
    header.image_type = (bytes_per_pixel == GRAYSCALE ? (rle ? RLE_GRAYSCALE : UNCOMPRESSED_GRAYSCALE) : (rle ? RLE_TRUE_COLOR : UNCOMPRESSED_TRUE_COLOR));
    header.image_descriptor = vertical_flip ? NONE : TOP_ORIGIN;

    out.write(reinterpret_cast<const char*>(&header), sizeof(header));
    if (!out.good()) {
        std::cerr << "TGAImage::write_tga_file: failed to write header" << std::endl;
        return false;
    }
    if (!rle) {
        out.write(reinterpret_cast<const char*>(data.data()), width * height * bytes_per_pixel);
        if (!out.good()) {
            std::cerr << "TGAImage::write_tga_file: failed to write data" << std::endl;
            return false;
        }
    }
    else if (!unload_rle_data(out)) {
        std::cerr << "TGAImage::write_tga_file: failed to write data" << std::endl;
        return false;
    }
    out.write(reinterpret_cast<const char*>(developer_area_ref), sizeof(developer_area_ref));
    if (!out.good()) {
        std::cerr << "TGAImage::write_tga_file: failed to write data" << std::endl;
        return false;
    }
    out.write(reinterpret_cast<const char*>(extension_area_ref), sizeof(extension_area_ref));
    if (!out.good()) {
        std::cerr << "TGAImage::write_tga_file: failed to write data" << std::endl;
        return false;
    }
    out.write(reinterpret_cast<const char*>(footer), sizeof(footer));
    if (!out.good()) {
        std::cerr << "TGAImage::write_tga_file: failed to write data" << std::endl;
        return false;
    }
    return true;
}

bool TGAImage::unload_rle_data(std::ofstream& out) const {
    const uint8_t max_chunk_length = 128;
    size_t number_of_pixels = width * height;
    size_t current_pixel = 0;
    while (current_pixel < number_of_pixels) {
        size_t chunk_start = current_pixel * bytes_per_pixel;
        size_t current_byte = current_pixel * bytes_per_pixel;
        uint8_t run_length = 1;
        bool raw = true;
        while (current_pixel + run_length < number_of_pixels && run_length < max_chunk_length) {
            bool succ_eq = true;
            for (int b = 0; succ_eq && b < bytes_per_pixel; b++) {
                succ_eq = (data[current_byte + b] == data[current_byte + b + bytes_per_pixel]);
            }
            current_byte += bytes_per_pixel;
            if (run_length == 1) {
                raw = !succ_eq;
            }
            if (!raw && !succ_eq) {
                break;
            }
            run_length++;
        }
        current_pixel += run_length;
        out.put(raw ? run_length - 1 : run_length + 127);
        if (!out.good()) {
            return false;
        }
        out.write(reinterpret_cast<const char*>(data.data() + chunk_start), (raw ? run_length * bytes_per_pixel : bytes_per_pixel));
        if (!out.good()) {
            std::cerr << "TGAImage::unload_rle_data: failed to write data" << std::endl;
            return false;
        }
    }
    return true;
}


TGAColor TGAImage::get(int x, int y) const {
    if (data.empty()) {
        std::cerr << "TGAImage::get: data is empty" << std::endl;
    }
    if (x < 0 || x >= width || y < 0 || y >= height) {
        std::cerr << "TGAImage::get: out of bounds" << std::endl;
    }

    TGAColor color = TGAColor(0, 0, 0, 0, bytes_per_pixel);
    const uint8_t* ptr_pixel = data.data() + (x + y * width) * bytes_per_pixel;
    for (int i = bytes_per_pixel - 1; i >= 0; i--) {
        color[i] = ptr_pixel[i];
    }
    return color;
}

void TGAImage::set(const int x, const int y, const TGAColor& color) {
    if (data.empty()) {
        std::cerr << "TGAImage::get: data is empty" << std::endl;
    }
    if (x < 0 || x >= width || y < 0 || y >= height) {
        std::cerr << "TGAImage::get: out of bounds" << std::endl;
    }
    memcpy(data.data() + (x + y * width) * bytes_per_pixel, color.bgra, bytes_per_pixel);
}

void TGAImage::flip_horizontally() {
    for (int i = 0; i < width / 2; i++) {
        for (int j = 0; j < height; j++) {
            for (int b = 0; b < bytes_per_pixel; b++) {
                std::swap(data[(i + j * width) * bytes_per_pixel + b], data[((j + 1) * width - 1 - i) * bytes_per_pixel + b]);
            }
        }
    }
}

void TGAImage::flip_vertically() {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height / 2; j++) {
            for (int b = 0; b < bytes_per_pixel; b++) {
                std::swap(data[(i + j * width) * bytes_per_pixel + b], data[(i + (height - 1 - j) * width) * bytes_per_pixel + b]);
            }
        }
    }
}

int TGAImage::get_width() const {
    return width;
}

int TGAImage::get_height() const {
    return height;
}