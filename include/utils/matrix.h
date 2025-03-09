#pragma once

/// @brief Matrix structure
typedef struct Matrix {
    int width;
    int height;
    double *data;
} Matrix;

extern Matrix create_matrix(int width, int height, double data[width][height]);