#include "transform.h"
#include "image.h"
#include <string.h>
#include <stdio.h>

bool rgb_to_gray(Image *dest, Image *src)
{
    char *type;
    if (strcmp(src->type, "P6") == 0) {
        type = "P5";
    } else if (strcmp(src->type, "P3") == 0) {
        type = "P2";
    } else {
        perror("Error during RGB to grey conversion: original image is not a valid RGB image");
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