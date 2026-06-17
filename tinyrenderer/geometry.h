//
// Created by vovac on 15.06.2026.
//
#pragma once

#ifndef TINYRENDERER_GEOMETRY_H
#define TINYRENDERER_GEOMETRY_H
#include <iostream>

template <int n>
struct vec {
    double data[n] = {0};
    double& operator[](const int i) { return data[i]; }
    double operator[](const int i) const { return data[i]; }
};

template <>
struct vec<3> {
    double x = 0, y = 0, z = 0;

    double& operator[](const int i) {
        if (i < 0 || i > 2) {
            throw std::out_of_range("vec3 index out of bounds");
        }
        if (i == 0) {
            return x;
        }
        if (i == 1) {
            return y;
        }
        return z;
    }

    double operator[](const int i) const {
        if (i < 0 || i > 2) {
            throw std::out_of_range("vec3 index out of bounds");
        }
        if (i == 0) {
            return x;
        }
        if (i == 1) {
            return y;
        }
        return z;
    }
};

typedef vec<3> vec3;

#endif //TINYRENDERER_GEOMETRY_H
