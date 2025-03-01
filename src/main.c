#include "image.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    const char * path = "../images/lena.ppm";

    Image image;
    load_image(path, &image);
    printf("Image of size (%d, %d, %d, %d)\n", image.max, image.width, image.height, image.channels);
    for (int i = 0; i<10; ++i) printf("%d ", *(image.content+i));
    printf("\n");

    FImage fimage;
    image_as_float(&fimage, &image);
    printf("FImage of size (%d, %d, %d)\n", fimage.width, fimage.height, fimage.channels);
    for (int i = 0; i<10; ++i) printf("%f ", *(fimage.content+i));
    printf("\n");
    
    return 0;
}
