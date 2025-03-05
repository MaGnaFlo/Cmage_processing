#include <stdio.h>
#include "server.h"

// temp
#include "image.h"
#include "transform.h"

int main(int argc, char **argv)
{

    Image image;
    load_image(&image, "../images/lena.ppm");
    Image grey;
    rgb_to_grey(&grey, &image);
    save_image(&grey, "tyest");

    const int port = 8888;
    struct MHD_Daemon *daemon;
    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, port, NULL, NULL,
                              &answer_to_connection, NULL, MHD_OPTION_END);
    if (daemon == NULL) {
        return 1;
    }

    (void) getchar();

    MHD_stop_daemon(daemon);
    return 0;
}
