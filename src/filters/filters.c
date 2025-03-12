#include "filters/filters.h"
#include "utils/matrix.h"
#include "image/image.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

bool filter(Image *dest, Image *src, Matrix *kernel)
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

static inline double gaussian2D(double x, double y, double sigma)
{
    return 1.0/sqrt(2*M_PI*sigma*sigma) * exp(-(pow(x,2)+pow(y,2))/2.0/sigma/sigma);
}

static Matrix create_gaussian_kernel(unsigned int kernel_size, double sigma)
{
    Matrix kernel = zero_matrix(kernel_size, kernel_size);
    double ux = (double)kernel_size/2, uy = (double)kernel_size/2;
    printf("%f %f %f\n", sigma, ux, uy);
    for (int i = 0; i < kernel_size; ++i) {
        for (int j = 0; j < kernel_size; ++j) {
            int x = i - kernel_size/2;
            int y = j - kernel_size/2;
            set_matrix_at(&kernel, i, j, gaussian2D((double)x, (double)y, sigma));
        }
    }
    return kernel;
}

bool gaussian_filter(Image *dest, Image *src, unsigned int kernel_size, double sigma)
{
    Matrix kernel = create_gaussian_kernel(kernel_size, sigma);
    bool rc = filter(dest, src, &kernel);
    free_matrix(&kernel);
    return rc;
}

bool sobel_filter(Image *grad_mag, Image *grad_angle, Image *src)
{
    Matrix sobel_x = create_matrix(3, 3, (double[3][3]){
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    });
    Matrix sobel_y = create_matrix(3, 3, (double[3][3]){
        {1, 2, 1},
        {0, 0, 0},
        {-1, -2, -1}
    });

    Image Ix, Iy, Ix_sq, Iy_sq, dI_sq;
    bool rc = true;
    rc = filter(&Ix, src, &sobel_x) && rc;
    rc = filter(&Iy, src, &sobel_y) && rc;
    rc = multiply_images(&Ix_sq, &Ix, &Ix) && rc;
    rc = multiply_images(&Iy_sq, &Iy, &Iy) && rc;
    rc = add_images(&dI_sq, &Ix_sq, &Iy_sq) && rc;
    rc = func_image(grad_mag, &dI_sq, &sqrt);
    

    // free
    free_image(&Ix);
    free_image(&Iy);
    free_image(&Ix_sq);
    free_image(&Iy_sq);
    free_image(&dI_sq);
    free_matrix(&sobel_x);
    free_matrix(&sobel_y);
}