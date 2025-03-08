#include "transform/geometry.h"
#include "image/image.h"
#include <stdio.h>
#include <string.h>
bool flip_horizontal(Image *dest, Image *src)
{
    create_image(dest, src->type, src->width, src->height, src->channels);
    if (!dest->content) {
        perror("Error during image allocation.");
        return false;
    }
    for (int col = 0; col < src->width; ++col) {
        for (int row = 0; row < src->height; ++row) {
            memcpy(pixel_at(dest, row, dest->width - col - 1), pixel_at(src, row, col), src->channels * sizeof(double));
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