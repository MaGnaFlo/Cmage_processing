#include "filters/filters.h"
#include "utils/matrix.h"
#include "image/image.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

bool filter2D(Image *dest, Image *src, Matrix *kernel)
{
    create_image(dest, src->type, src->width, src->height, src->channels);
    if (!dest->content) {
        perror("Error during image allocation.");
        return false;
    }

    double total_weight = 0;
    for (int i = 0; i < kernel->height; i++) {
        for (int j = 0; j < kernel->width; j++) {
            total_weight += fabs(matrix_at(kernel, i, j));
        }
    }

    for (int row = 0; row < src->height; ++row) {
        for (int col = 0; col < src->width; ++col) {
            double *pixel_dest = pixel_at(dest, col, row);
            for (int i = 0; i < kernel->height; ++i) {
                int row_i = row - (i - kernel->height / 2);
                if (row_i < 0 || row_i >= src->height) continue;
                for (int j = 0; j < kernel->width; ++j) {
                    int col_j = col - (j - kernel->width / 2);
                    if (col_j < 0 || col_j >= src->height) continue;
                    double *pixel_src = pixel_at(src, col_j, row_i);
                    for (int c = 0; c < src->channels; ++c) {
                        *(pixel_dest+c) += *(pixel_src+c) * matrix_at(kernel, i, j) / total_weight;
                    }
                }
            }
        }
    }
    return true;
}