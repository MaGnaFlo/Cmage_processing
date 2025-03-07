#include <string.h>
#include <stdio.h>
#include "image/image.h"
#include "transform/colors.h"
#include <math.h>

bool rgb_to_gray(Image *dest, Image *src)
{
    char *type;
    if (strcmp(src->type, "P6") == 0) {
        type = "P5";
    } else if (strcmp(src->type, "P3") == 0) {
        type = "P2";
    } else {
        perror("Error during RGB to gray conversion: original image is not a valid RGB image");
        return false;
    }
    
    create_image(dest, type, src->width, src->height, 1);
    if (!dest->content) {
        perror("Error during image allocation.");
        return false;
    }

    for (int i = 0; i<src->width; ++i) {
        for (int j = 0; j<src->height; ++j) {
            int pixel_pos = j*src->channels + i*src->channels*src->height;
            dest->content[j+i*src->height] = (src->content[pixel_pos]+src->content[pixel_pos+1]+src->content[pixel_pos+2])/3.0;
        }
    }
    return true;
}

bool gray_to_rgb(Image *dest, Image *src)
{
    char *type;
    if (strcmp(src->type, "P5") == 0) {
        type = "P6";
    } else if (strcmp(src->type, "P2") == 0) {
        type = "P3";
    } else {
        perror("Error during gray to RGB conversion: original image is not a valid gray (one channel) image");
        return false;
    }
    
    create_image(dest, type, src->width, src->height, 3);
    if (!dest->content) {
        perror("Error during image allocation.");
        return false;
    }

    for (int i = 0; i<dest->width; ++i) {
        for (int j = 0; j<dest->height; ++j) {
            int pixel_pos = j*dest->channels + i*dest->channels*src->height;
            double value = src->content[j+i*dest->height];
            dest->content[pixel_pos] = value;
            dest->content[pixel_pos+1] = value;
            dest->content[pixel_pos+2] = value;
        }
    }
    return true;
}

static void rgb_to_hsv_pixel(double *dest, double *src)
{
    double r = *src, g = *(src+1), b = *(src+2);
    double h = 0, s = 0, v = 0;

    // value & chroma
    double max_value = fmax(fmax(r,g),b);
    double min_value = fmin(fmin(r,g),b);
    double chroma = max_value - min_value;
    v = max_value;

    // saturation
    if (v != 0) s = chroma / v;
    
    // hue
    if (chroma != 0) {
        if (v == r) {
            h = (g-b) / chroma;
            if (h < 0) h += 6;
        } else if (v == g) {
            h = (b-r) / chroma + 2;
        } else {
            h = (r-g) / chroma + 4;
        }
    }
    h *= 60.0;
    h = fmod(h, 360);
    if (h < 0) h += 360;

    *dest = h;
    *(dest+1) = s;
    *(dest+2) = v;
}

bool rgb_to_hsv(Image *dest, Image *src)
{
    create_image(dest, src->type, src->width, src->height, src->channels);
    if (!dest->content) {
        perror("Error during image allocation.");
        return false;
    }

    bool ok = true;
    for (int col = 0; col<src->width; ++col) {
        for (int row = 0; row<src->height; ++row) {
            double * src_pixel = pixel_at(src, row, col);
            double * dest_pixel = pixel_at(dest, row, col);
            if (!src_pixel || !dest_pixel) {
                ok = false;
                continue;
            }
            rgb_to_hsv_pixel(dest_pixel, src_pixel);
        }
    }
    return ok;
}

static void hsv_to_rgb_pixel(double *dest, double *src)
{
    double h = *src, s = *(src+1), v = *(src+2);
    h /= 60.0;
    double r = 0, g = 0, b = 0;

    double chroma = v * s;
    double x = chroma * (1 - fabs(fmod(h,2) - 1));
    int h_ = (int)floor(h);
    if (h_ < 0) h_ += 6; // just to be sure
    
    switch (h_) {
        case 0: r = chroma; g = x; break;
        case 1: r = x; g = chroma; break;
        case 2: g = chroma; b = x; break;
        case 3: g = x; b = chroma; break;
        case 4: r = x; b = chroma; break;
        case 5: r = chroma; b = x; break;
    }

    double m = v - chroma;
    *dest = r + m;
    *(dest+1) = g + m;
    *(dest+2) = b + m;
}

bool hsv_to_rgb(Image *dest, Image *src)
{
    create_image(dest, src->type, src->width, src->height, src->channels);
    if (!dest->content) {
        perror("Error during image allocation.");
        return false;
    }

    bool ok = true;
    for (int col = 0; col<src->width; ++col) {
        for (int row = 0; row<src->height; ++row) {
            double * src_pixel = pixel_at(src, row, col);
            double * dest_pixel = pixel_at(dest, row, col);
            if (!src_pixel || !dest_pixel) {
                ok = false;
                continue;
            }
            hsv_to_rgb_pixel(dest_pixel, src_pixel);
        }
    }
    return ok;
}

void shift_hue(Image *src, int shift)
{
    for (int col = 0; col<src->width; ++col) {
        for (int row = 0; row<src->height; ++row) {
            double *h = pixel_at(src, row, col);
            *h = *h + shift;
            if (*h < 0) *h += 360;
            *h = fmod(*h, 360);
        }
    }
}
