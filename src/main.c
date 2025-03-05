#include <stdio.h>
#include "server.h"
#include <microhttpd.h>

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
