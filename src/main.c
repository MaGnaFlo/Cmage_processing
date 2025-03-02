#include <stdio.h>
#include "server.h"
#include "image.h"

#define PORT 8888

int main(int argc, char **argv)
{
    // const char * path = "../images/lena.ppm";

    // Image image;
    // (void) load_image(path, &image);
    // printf("Image of size (%d, %d, %d, %d)\n", image.max, image.width, image.height, image.channels);
    // for (int i = 0; i<10; ++i) printf("%d ", *(image.content+i));
    // printf("\n");

    // FImage fimage;
    // (void) to_FImage(&fimage, &image);
    // printf("FImage of size (%d, %d, %d)\n", fimage.width, fimage.height, fimage.channels);
    // for (int i = 0; i<10; ++i) printf("%f ", *(fimage.content+i));
    // printf("\n");

    // // png
    // image_to_png(&image, "res.png");

    // // write
    // save_image("saved", &image);
    
    struct MHD_Daemon *daemon;
    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                              &answer_to_connection, NULL, MHD_OPTION_END);
    if (daemon == NULL) {
        return 1;
    }

    (void) getchar();

    MHD_stop_daemon(daemon);
    return 0;
}
