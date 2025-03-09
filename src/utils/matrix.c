#include "utils/matrix.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

Matrix zero_matrix(unsigned int width, unsigned int height)
{
    Matrix mat;
    mat.width = width;
    mat.height = height;
    mat.data = (double *)malloc(width * height * sizeof(double));
    return mat;
}

Matrix create_matrix(unsigned int width, unsigned int height, double data[width][height])
{
    Matrix mat = zero_matrix(width, height);
    for (int j = 0; j < height; ++j) {
        memcpy(mat.data + j * width, data[j], width * sizeof(double));
    }
    return mat;
}

double matrix_at(Matrix *mat, unsigned int i, unsigned int j)
{
    if (i >= mat->width || j >= mat->height) {
        fprintf(stderr, "Accessing matrix value of out bounds (%d %d for size %dx%d)", i, j, mat->width, mat->height);
        return 0;
    }
    return *(mat->data + i * mat->width + j);
}

void set_matrix_at(Matrix *mat, unsigned int i, unsigned int j, double val)
{
    if (i >= mat->width || j >= mat->height) {
        fprintf(stderr, "Accessing matrix value of out bounds (%d %d for size %dx%d)", i, j, mat->width, mat->height);
    }
    *(mat->data + i * mat->width + j) = val;
}

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
                value += matrix_at(A, i, k) * matrix_at(B, k, j);
            }
            set_matrix_at(&res, i, j, value);
        }
    }
    return res;
}

Matrix submatrix(Matrix *mat, unsigned int i_ex, unsigned int j_ex)
{
    Matrix submat = zero_matrix(mat->width-1, mat->height-1);
    unsigned int i_sub = 0;
    for (int i = 0; i < mat->width; ++i) {
        unsigned int j_sub = 0;
        if (i == i_ex) continue;
        for (int j = 0; j < mat->height; ++j) {
            if (j == j_ex) continue;
            set_matrix_at(&submat, i_sub, j_sub, matrix_at(mat, i, j));
            ++j_sub;
        }
        ++i_sub;
    }
    return submat;
}

double determinant(Matrix *mat)
{
    double det_value = 0;
    if (mat->width == 2) {
        det_value = matrix_at(mat, 0, 0) * matrix_at(mat, 1, 1) - matrix_at(mat, 0, 1) * matrix_at(mat, 1, 0);
    } else {
        for (int i = 0; i < mat->height; ++i) {
            Matrix submat = submatrix(mat, i, 0);
            det_value += pow(-1, i) * matrix_at(mat, i, 0) * determinant(&submat);
        }
    }
    return det_value;
}

Matrix comatrix(Matrix *mat)
{
    Matrix comat = zero_matrix(mat->width, mat->height);
    for (int i = 0; i < mat->width; ++i) {
        for (int j = 0; j < mat->height; ++j) {
            Matrix submat = submatrix(mat, i, j);
            set_matrix_at(&comat, i, j, pow(-1, i+j) * determinant(&submat));
        }
    }
    return comat;
}

Matrix transpose(Matrix *mat)
{
    Matrix t_mat = zero_matrix(mat->width, mat->height);
    for (int i = 0; i < mat->width; ++i) {
        for (int j = 0; j < mat->height; ++j) {
            set_matrix_at(&t_mat, i, j, matrix_at(mat, j, i));
        }
    }
    return t_mat;
}

Matrix inverse(Matrix *mat)
{
    Matrix inverse;
    double det = determinant(mat);
    if (det == 0) {
        perror("Matrix has a null determinant; inverse matrix cannot be computed.");
        inverse.width = 0;
        inverse.height = 0;
        return inverse;
    }

    inverse = zero_matrix(mat->width, mat->height);
    Matrix comat = comatrix(mat);
    Matrix t_comat = transpose(&comat);
    for (int i = 0; i < inverse.width; ++i) {
        for (int j = 0; j < inverse.height; ++j) {
            set_matrix_at(&inverse, i, j, matrix_at(&t_comat, i, j) / det);
        }
    }
    return inverse;
}

void print_matrix(Matrix *mat)
{
    printf("Size: (%d,%d)\n", mat->width, mat->height);
    for (int i = 0; i<mat->width; i++) {
        for (int j = 0; j<mat->height; j++)
        printf("%f ", matrix_at(mat, i, j));
        printf("\n");
    }
}