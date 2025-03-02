#include "image.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    const char * path = "../images/lena.ppm";

    Image image;
    (void) load_image(path, &image);
    printf("Image of size (%d, %d, %d, %d)\n", image.max, image.width, image.height, image.channels);
    for (int i = 0; i<10; ++i) printf("%d ", *(image.content+i));
    printf("\n");

    FImage fimage;
    (void) to_FImage(&fimage, &image);
    printf("FImage of size (%d, %d, %d)\n", fimage.width, fimage.height, fimage.channels);
    for (int i = 0; i<10; ++i) printf("%f ", *(fimage.content+i));
    printf("\n");

    // png
    image_to_png(&image, "res.png");

    // write
    save_image("saved", &image);
    
    return 0;
}
