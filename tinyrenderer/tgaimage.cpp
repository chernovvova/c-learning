#include "tgaimage.h"

TGAImage::TGAImage(const int width, const int height, const int bytes_per_pixel) {
    this->width = width;
    this->height = height;
    this->bytes_per_pixel = bytes_per_pixel;
}

TGA