//
// Created by vovac on 15.06.2026.
//
#pragma once

#ifndef TINYRENDERER_GEOMETRY_H
#define TINYRENDERER_GEOMETRY_H
#include <iostream>
#include "tgaimage.h"

template <int N, typename T>
struct vec {
    T data[N] = {0};
    T& operator[](const T i) { return data[i]; }
    T operator[](const T i) const { return data[i]; }
};

template <int N, typename T>
vec<N, T> operator+(vec<N, T> a, vec<N, T> b) {
    for (int i = 0; i < N; i++) a[i] = b[i];
    return a;
}

template <int N, typename T>
vec<N, T> operator*(vec<N, T> a, T s) {
    for (int i = 0; i < N; i++) a[i] *= s;
    return a;
}

template <int N, typename T>
T dot(vec<N, T> a, vec<N, T> b) {
    T r = 0;
    for (int i = 0; i < N; i++) r += a[i] * b[i];
    return r;
}

template <typename T>
struct vec<2, T> {
    T x = 0, y = 0;
    T operator[](int i) const { return i == 0 ? x : y; }
    T& operator[](int i) { return i == 0 ? x : y;}
};

template <typename T>
struct vec<3, T> {
    T x = 0, y = 0, z = 0;
    T operator[](int i) const { return i == 0 ? x : (i == 1 ? y : z); }
    T& operator[](int i) { return i == 0 ? x : (i == 1 ? y : z); }
};


template <typename T>
struct vec<4, T> {
    T x = 0, y = 0, z = 0, w = 0;
    T operator[](int i) const { return i == 0 ? x : (i == 1 ? y : (i == 2) ? z : w); }
    T& operator[](int i) { return i == 0 ? x : (i == 1 ? y : (i == 2) ? z : w); }
};

typedef vec<2, int> vec2i;
typedef vec<3, int> vec3i;

typedef vec<3, double> vec3d;

double triangle_area(vec2i a, vec2i b, vec2i c);
void line(vec2i a, vec2i b, TGAImage &framebuffer, TGAColor color);
void triangle(vec3i a, vec3i b, vec3i c, TGAImage &framebuffer, TGAImage &zbuffer, TGAColor color);

vec3i projection(const vec3d& vector, int width, int height);

#endif //TINYRENDERER_GEOMETRY_H
