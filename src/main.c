#include <stdio.h>
#include <microhttpd.h>
#include "server/server.h"

// temp ////////////////////////////
#include "image/image.h"
#include "transform/geometry.h"
#include "transform/colors.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "utils/matrix.h"
#include "filters/filters.h"
///////////////////////////////////

int main(int argc, char **argv)
{
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
