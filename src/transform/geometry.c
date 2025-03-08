#include "transform/geometry.h"
#include "image/image.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

/// @brief Nearest neighbors interpolation of a pixel
/// @param pixel_dest Pixel to interpolate
/// @param src Source image
/// @param col 
/// @param row 
static void nearest_neighbors_interpolation(double *pixel_dest, Image *src, int col, int row)
{
    if (col >= 0 && col < src->width && row >= 0 && row < src->height) {
        memcpy(pixel_dest, pixel_at(src, col, row), src->channels * sizeof(double));
    }
}

/// @brief Bilinear interpolation of a pixel
/// @param pixel_dest Pixel to interpolate
/// @param src Source image
/// @param col Floating point col coordinate
/// @param row Floating point row coordinate
static void bilinear_interpolation(double *pixel_dest, Image *src, double col, double row)
{
    // neighbors
    int col0 = (int)floor(col);
    int col1 = (int)ceil(col);
    int row0 = (int)floor(row);
    int row1 = (int)ceil(row);
    if (col0 < 0 || col1 >= src->width || row0 < 0 || row1 >= src->height) {
        return;
    }
    // deltas
    double dcol = col - col0;
    double drow = row - row0;
    // horizontal interpolation
    double *pixel_top = (double *)malloc(src->channels * sizeof(double));
    for (int c = 0; c < src->channels; ++c) {
        *(pixel_top+c) = *(pixel_at(src, col0, row0)+c) * (1-dcol) + *(pixel_at(src, col1, row0)+c) * dcol;
    }
    double *pixel_bottom = (double *)malloc(src->channels * sizeof(double));
    for (int c = 0; c < src->channels; ++c) {
        *(pixel_bottom+c) = *(pixel_at(src, col0, row1)+c) * (1-dcol) + *(pixel_at(src, col1, row1)+c) * dcol;
    }
    // vertical interpolation
    for (int c = 0; c < src->channels; ++c) {
        *(pixel_dest+c) = (1-drow) * *(pixel_top+c) + drow * *(pixel_bottom+c);
    }
}

bool flip_horizontal(Image *dest, Image *src)
{
    create_image(dest, src->type, src->width, src->height, src->channels);
    if (!dest->content) {
        perror("Error during image allocation.");
        return false;
    }
    for (int col = 0; col < src->width; ++col) {
        for (int row = 0; row < src->height; ++row) {
            memcpy(pixel_at(dest, dest->width - col - 1, row), pixel_at(src, col, row), src->channels * sizeof(double));
        }
    }
    return true;
}

bool flip_vertical(Image *dest, Image *src)
{
    create_image(dest, src->type, src->width, src->height, src->channels);
    if (!dest->content) {
        perror("Error during image allocation.");
        return false;
    }
    for (int col = 0; col < src->width; ++col) {
        memcpy(dest->content + col * dest->height * dest->channels,
            src->content + (src->width - col - 1) * src->height * src->channels,
            src->height * src->channels * sizeof(double));
        }
        return true;
    }
    
    bool resize(Image *dest, Image *src, int width, int height, INTERP interp)
    {
        create_image(dest, src->type, width, height, src->channels);
        if (!dest->content) {
            perror("Error during image allocation.");
            return false;
        }
        
        for (int col = 0; col < width; ++col) {
            for (int row = 0; row < height; ++row) {
                double *pixel = pixel_at(dest, col, row);
                double col_src = col*(double)src->width/width;
                double row_src = row*(double)src->height/height;
                switch (interp) {
                    case INTERP_NEAREST: {
                        nearest_neighbors_interpolation(pixel, src, (int)col_src, (int)row_src);
                        break;
                    }
                    case INTERP_BILINEAR: {
                        bilinear_interpolation(pixel, src, col_src, row_src);
                        break;
                    }
                }
            }
        }
        return true;
    }

    bool rotate(Image *dest, Image *src, double angle, INTERP interp)
    {
        int width = (int)(src->width * cos(angle) + src->height * sin(angle));
        int height = (int)(src->width * sin(angle) + src->height * cos(angle));
        
        create_image(dest, src->type, width, height, src->channels);
        if (!dest->content) {
            perror("Error during image allocation.");
            return false;
        }

        double cx = (double)src->width / 2;
        double cy = (double)src->height / 2;
        double dest_cx = width / 2;
        double dest_cy = height / 2;
        
        for (int col = 0; col < width; ++col) {
            for (int row = 0; row < height; ++row) {
                double *pixel = pixel_at(dest, col, row);
                double col_src = (col-dest_cx)*cos(-angle) + (row-dest_cy)*sin(-angle) + cx;
                double row_src = -(col-dest_cx)*sin(-angle) + (row-dest_cy)*cos(-angle) + cy;
                switch (interp) {
                    case INTERP_NEAREST: {
                        nearest_neighbors_interpolation(pixel, src, (int)col_src, (int)row_src);
                        break;
                    }
                    case INTERP_BILINEAR: {
                        bilinear_interpolation(pixel, src, col_src, row_src);
                        break;
                    }
                }
            }
        }
        return true;
    }