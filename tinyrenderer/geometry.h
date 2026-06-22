//
// Created by vovac on 15.06.2026.
//
#pragma once

#ifndef TINYRENDERER_GEOMETRY_H
#define TINYRENDERER_GEOMETRY_H
#include "tgaimage.h"

template <int N, typename T>
struct vec {
    T data[N] = {0};
    T& operator[](const int i) { return data[i]; }
    T operator[](const int i) const { return data[i]; }
};

template <int N, typename T>
vec<N, T> operator+(vec<N, T> a, vec<N, T> b) {
    vec<N, T> result;
    for (int i = 0; i < N; i++) result[i] = a[i] + b[i];
    return a;
}

template <int N, typename T>
vec<N, T> operator*(vec<N, T> a, T s) {
    vec<N, T> result;
    for (int i = 0; i < N; i++) result[i] = a[i] * s;
    return a;
}

template <int N, typename T>
vec<N, T> operator/(vec<N, T> a, T s) {
    vec<N, T> result;
    for (int i = 0; i < N; i++) result[i] = a[i] / s;
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

template <int N, int M, typename T>
struct mat {
    vec<M, T> data[N] = {{}};
    vec<M, T>& operator[](const int i) { return data[i]; }
    vec<M, T> operator[](const int i) const { return data[i]; }

    mat<M, N, T> transpose() {
        mat<M, N, T> transposed;
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                transposed[i][j] = data[j][i];
            }
        }
        return transposed;
    }
};

template <int N, int M, typename T>
mat<N, M, T> operator*(mat<N, M, T> matrix, const T& val) {
    mat<N, M, T> result;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            result[i][j] = matrix[i][j] * val;
        }
    }
    return result;
}

template <int N, int M, typename T>
mat<N, M, T> operator/(mat<N, M, T> matrix, const T& val) {
    mat<N, M, T> result;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            result[i][j] = matrix[i][j] / val;
        }
    }
    return result;
}

template <int N, int M, typename T>
mat<N, M, T> operator+(mat<N, M, T> matrix, mat<N, M, T> other_matrix) {
    mat<N, M, T> result;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            result[i][j] = matrix[i][j] + other_matrix[i][j];
        }
    }
    return result;
}

template <int N, int M, typename T>
mat<N, M, T> operator-(mat<N, M, T> matrix, mat<N, M, T> other_matrix) {
    mat<N, M, T> result;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            result[i][j] = matrix[i][j] - other_matrix[i][j];
        }
    }
    return result;
}

template <int N, int M, typename T>
mat<N, N, T> operator*(mat<N, M, T> matrix, mat<M, N, T> other_matrix) {
    mat<N, N, T> result {{}};
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
           for (int k = 0; k < N; k++) {
               result[i][j] += matrix[i][k] * other_matrix[k][j];
           }
        }
    }
    return result;
}

template <int N, int M, typename T>
vec<N, T> operator*(mat<N, M, T> matrix, vec<M, T> vector) {
    vec<M, T> result = {};
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            result[i] += matrix[i][j] * vector[j];
        }
    }
    return result;
}

template <typename T>
T det(mat<2, 2, T> matrix) {
    return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
}

template <typename T>
T det(mat<3, 3, T> matrix) {
    return matrix[0][0] * matrix[1][1] * matrix[2][2]
            - matrix[0][0] * matrix[1][2] * matrix[2][1]
            - matrix[0][1] * matrix[1][0] * matrix[2][2]
            + matrix[0][1] * matrix[1][2] * matrix[2][0]
            + matrix[0][2] * matrix[1][0] * matrix[2][1]
            - matrix[0][2] * matrix[1][1] * matrix[2][0];
}

template <typename T>
T det(mat<4, 4, T> matrix) {
    T det00 = matrix[1][1]*(matrix[2][2]*matrix[3][3] - matrix[2][3]*matrix[3][2])
      - matrix[1][2]*(matrix[2][1]*matrix[3][3] - matrix[2][3]*matrix[3][1])
      + matrix[1][3]*(matrix[2][1]*matrix[3][2] - matrix[2][2]*matrix[3][1]);
    T det01 = matrix[1][0]*(matrix[2][2]*matrix[3][3] - matrix[2][3]*matrix[3][2])
          - matrix[1][2]*(matrix[2][0]*matrix[3][3] - matrix[2][3]*matrix[3][0])
          + matrix[1][3]*(matrix[2][0]*matrix[3][2] - matrix[2][2]*matrix[3][0]);
    T det02 = matrix[1][0]*(matrix[2][1]*matrix[3][3] - matrix[2][3]*matrix[3][1])
          - matrix[1][1]*(matrix[2][0]*matrix[3][3] - matrix[2][3]*matrix[3][0])
          + matrix[1][3]*(matrix[2][0]*matrix[3][1] - matrix[2][1]*matrix[3][0]);
    T det03 = matrix[1][0]*(matrix[2][1]*matrix[3][2] - matrix[2][2]*matrix[3][1])
          - matrix[1][1]*(matrix[2][0]*matrix[3][2] - matrix[2][2]*matrix[3][0])
          + matrix[1][2]*(matrix[2][0]*matrix[3][1] - matrix[2][1]*matrix[3][0]);

    return matrix[0][0]*det00 - matrix[0][1]*det01 + matrix[0][2]*det02 - matrix[0][3]*det03;
}

template <typename T>
mat<2, 2, T> inverse(mat<2, 2, T> matrix) {
    T determinant = det(matrix);
    return {
        {matrix[1][1] / determinant, - matrix[0][1] / determinant},
        { - matrix[1][0] / determinant, matrix[0][0] / determinant}
    };
}

template <typename T>
mat<3, 3, T> inverse(mat<3, 3, T> matrix) {
    T determinant = det(matrix);
    mat<3, 3, T> inverse;
    inverse[0][0] = (matrix[1][1]*matrix[2][2] - matrix[1][2]*matrix[2][1]) / determinant;
    inverse[0][1] = (matrix[0][2]*matrix[2][1] - matrix[0][1]*matrix[2][2]) / determinant;
    inverse[0][2] = (matrix[0][1]*matrix[1][2] - matrix[0][2]*matrix[1][1]) / determinant;

    inverse[1][0] = (matrix[1][2]*matrix[2][0] - matrix[1][0]*matrix[2][2]) / determinant;
    inverse[1][1] = (matrix[0][0]*matrix[2][2] - matrix[0][2]*matrix[2][0]) / determinant;
    inverse[1][2] = (matrix[0][2]*matrix[1][0] - matrix[0][0]*matrix[1][2]) / determinant;

    inverse[2][0] = (matrix[1][0]*matrix[2][1] - matrix[1][1]*matrix[2][0]) / determinant;
    inverse[2][1] = (matrix[0][1]*matrix[2][0] - matrix[0][0]*matrix[2][1]) / determinant;
    inverse[2][2] = (matrix[0][0]*matrix[1][1] - matrix[0][1]*matrix[1][0]) / determinant;

    return inverse;
}

double triangle_area(vec2i a, vec2i b, vec2i c);
void line(vec2i a, vec2i b, TGAImage &framebuffer, TGAColor color);
void triangle(vec3i a, vec3i b, vec3i c, TGAImage &framebuffer, TGAImage &zbuffer, TGAColor color);

vec3i projection(const vec3d& vector, int width, int height);

#endif //TINYRENDERER_GEOMETRY_H
