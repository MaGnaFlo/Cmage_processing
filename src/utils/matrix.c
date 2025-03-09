#include "utils/matrix.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/// @brief Instanciates a matrix
/// @param width Matrix width
/// @param height Matrix height
/// @param data Matrix array representation
/// @return 
Matrix create_matrix(int width, int height, double data[width][height])
{
    Matrix mat;
    mat.width = width;
    mat.height = height;
    mat.data = (double *)malloc(width * height * sizeof(double));
    for (int j = 0; j < height; ++j) {
        memcpy(mat.data + j * width, data[j], width * sizeof(double));
    }
    return mat;
}

/// @brief Matrix multiplication
/// @param A Left matrix
/// @param B Right matrix
/// @return
Matrix matmul(Matrix *A, Matrix *B)
{
    Matrix res;
    if (A->width != B->height) {
        perror("Matrix sizes not suitable for multiplication");
        return res;
    }
    res.width = B->width;
    res.height = A->height;
    res.data = (double *)malloc(res.width * res.height * sizeof(double));

    for (int i = 0; i < A->height; ++i) {
        for (int j = 0; j < B->width; ++j) {
            double value = 0;
            for (int k = 0; k < res.height; ++k) {
                value += *(A->data + i * A->width + k) * *(B->data + k * B->width + j);
            }
            *(res.data + i * res.width + j) = value;
        }
    }
    return res;
}