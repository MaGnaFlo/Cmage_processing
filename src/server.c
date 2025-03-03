#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdarg.h>
#include "server.h"
#include "image.h"

#define IMAGES_PATH "../images/"
#define INDEX "../front/index.html"
#define SCRIPTS "../front/scripts.js"
#define MIME_TEXT "text/plain"
#define MIME_HTML "text/html"
#define MIME_SCRIPT "application/javascript"
#define MIME_PNG "image/png"
#define ERROR_PAGE "<html><body>An internal server error has occurred!</body></html>"
#define TEMP_LOADED_IMG "loaded_img.png"

/// @brief Creates a response given a request
/// @param connection Connection
/// @param content_type MIME content type, if any
/// @param status_code HTTP return status
/// @param response_type Type of function to use for request
/// @param argcount Number of additional arguments
/// @param ... additionnal arguments
/// @return 
static
enum MHD_Result
create_response(struct MHD_Connection *connection,
                const char *content_type,
                int status_code, 
                const char *response_type,
                int argcount, ...)
{
    int ret;
    va_list args;
    va_start(args, argcount);
    struct MHD_Response *response = NULL;
    if (strcmp(response_type, "from_buffer") == 0 && argcount == 3) {
        size_t size = va_arg(args, size_t);
        char *buffer = va_arg(args, char *);
        enum MHD_ResponseMemoryMode mem_mode = va_arg(args, enum MHD_ResponseMemoryMode);
        response = MHD_create_response_from_buffer(size, buffer, mem_mode);
    } else if (strcmp(response_type, "from_fd") == 0 && argcount == 3) {
        size_t size = va_arg(args, size_t);
        int fd = va_arg(args, int);
        off_t offset = va_arg(args, off_t);
        response = MHD_create_response_from_fd_at_offset64(size, fd, offset);
    } else {
        return MHD_NO;
    }

    if (response) {
        MHD_add_response_header(response, "Content-Type", content_type);
        ret = MHD_queue_response(connection, status_code, response);
        MHD_destroy_response(response);
    } else {
        ret = MHD_NO;
    }
    return ret;
}

static
enum MHD_Result
answer_to_root(struct MHD_Connection *connection)
{
    struct MHD_Response *response;
    int fd, ret;
    struct stat sbuf;
    
    if (-1 == (fd = open(INDEX, O_RDONLY)) || (0 != fstat(fd, &sbuf))) {
        // error accessing file
        if (fd != -1) {
            close(fd);
        }
        ret = create_response(connection, MIME_TEXT, MHD_HTTP_INTERNAL_SERVER_ERROR, "from_buffer", 
                              3, strlen(ERROR_PAGE), (void*)ERROR_PAGE, MHD_RESPMEM_PERSISTENT);
    }

    char *buffer = (char*)malloc(sbuf.st_size);
    read(fd, buffer, sbuf.st_size);
    close(fd);

    ret = create_response(connection, MIME_HTML, MHD_HTTP_OK, "from_buffer", 
                          3, sbuf.st_size, buffer, MHD_RESPMEM_MUST_FREE);
    return ret;
}

static
enum MHD_Result
answer_to_script(struct MHD_Connection *connection)
{
    struct MHD_Response *response;
    int fd, ret;
    struct stat sbuf;
    
    if (-1 == (fd = open(SCRIPTS, O_RDONLY)) || (0 != fstat(fd, &sbuf))) {
        // error accessing file
        if (fd != -1) {
            close(fd);
        }
        ret = create_response(connection, MIME_TEXT, MHD_HTTP_INTERNAL_SERVER_ERROR, "from_buffer", 
                              3, strlen(ERROR_PAGE), (void*)ERROR_PAGE, MHD_RESPMEM_PERSISTENT);
    }

    char *buffer = (char*)malloc(sbuf.st_size);
    read(fd, buffer, sbuf.st_size);
    close(fd);

    ret = create_response(connection, MIME_HTML, MHD_HTTP_OK, "from_buffer", 3, sbuf.st_size, buffer, MHD_RESPMEM_MUST_FREE);
    return ret;
}

static
enum MHD_Result
answer_to_image(struct MHD_Connection *connection, const char *url)
{
    struct MHD_Response *response;
    int fd, ret;
    struct stat sbuf;
    
    // build image path from received url
    char *img_name = strrchr(url, '/');
    img_name++;
    char *relative_path = (char*)malloc(strlen(IMAGES_PATH));
    strcpy(relative_path, IMAGES_PATH);
    strcat(relative_path, img_name);

    // we actually have to perform a conversion since HTML is not happy with PPM/PGM
    Image image;
    if (!load_image(relative_path, &image)) {
        perror("Could not properly load image.");
        return MHD_NO;
    }
    if (image_to_png(&image, TEMP_LOADED_IMG)) {
        perror("An error occurred during PNG conversion.");
        return MHD_NO;
    }
    free_image(&image);
    
    if (-1 == (fd = open(TEMP_LOADED_IMG, O_RDONLY)) || (0 != fstat(fd, &sbuf))) {
        // error accessing file
        if (fd != -1) {
            close(fd);
        }
        ret = create_response(connection, MIME_TEXT, MHD_HTTP_INTERNAL_SERVER_ERROR, "from_buffer", 
                              3, strlen(ERROR_PAGE), (void*)ERROR_PAGE, MHD_RESPMEM_PERSISTENT);
    }
    
    ret = create_response(connection, MIME_PNG, MHD_HTTP_OK, "from_fd", 3, sbuf.st_size, fd, 0);
    return ret;
}

static
enum MHD_Result
answer_to_unknown(struct MHD_Connection *connection)
{
    int ret;
    const char *empty = "";
    struct MHD_Response *response;
    ret = create_response(connection, MIME_TEXT, MHD_HTTP_INTERNAL_SERVER_ERROR, "from_buffer", 
                          3, 0, (void *)empty, MHD_RESPMEM_PERSISTENT);
    return ret;
}

enum MHD_Result
answer_to_connection(void *cls,
    struct MHD_Connection *connection,
    const char *url,
    const char *method,
    const char *version,
    const char *upload_data,
    size_t *upload_data_size,
    void **con_cls)
    {
        (void) version;
        (void) upload_data;
        (void) upload_data_size;
        (void) con_cls;

        int ret;
        if (strcmp(url, "/") == 0) {
            ret = answer_to_root(connection);
        } else if (strcmp(url, "/scripts.js") == 0) {
            ret = answer_to_script(connection);
        } else if (strstr(url, "image") != NULL) {
            ret = answer_to_image(connection, url);
        } else {
            ret = answer_to_unknown(connection);
        }
        return ret;
    }
    