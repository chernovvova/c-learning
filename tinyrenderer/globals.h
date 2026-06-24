//
// Created by vovac on 24.06.2026.
//

#ifndef TINYRENDERER_GLOBALS_H
#define TINYRENDERER_GLOBALS_H
#include "tgaimage.h"

inline int WIDTH = 800;
inline int HEIGHT = 800;

inline constexpr vec3d eye = {-1, 0, 2};
inline constexpr vec3d center = {0, 0, 0};
inline constexpr vec3d up = {0, 1, 0};

inline mat<4, 4, double> ModelView;
inline mat<4, 4, double> Viewport;
inline mat<4, 4, double> Perspective;

const TGAColor white = {255, 255, 255, 255};
const TGAColor green = {0, 255, 0, 255};
const TGAColor red = {255, 0, 0, 255};
const TGAColor blue = {0, 0, 255, 255};
const TGAColor grey = {128, 128, 128, 255};

#endif //TINYRENDERER_GLOBALS_H