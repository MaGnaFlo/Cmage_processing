#include "transform/geometry.h"
#include "image/image.h"
#include <stdio.h>

bool flip_horizontal(Image *dest, Image *src)
{
    create_image(dest, src->type, src->width, src->height, src->channels);
    if (!dest->content) {
        perror("Error during image allocation.");
        return false;
    }
    for (int col = 0; col < src->width; ++col) {
        for (int row = 0; row < src->height; ++row) {
            for (int c = 0; c < src->channels; c++) {
                *(pixel_at(dest, row, dest->width - col - 1) + c) = *(pixel_at(src, row, col) + c);
            }
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
        for (int row = 0; row < src->height; ++row) {
            for (int c = 0; c < src->channels; c++) {
                *(pixel_at(dest, dest->height - row - 1,  col) + c) = *(pixel_at(src, row, col) + c);
            }
        }
    }
    return true;
}