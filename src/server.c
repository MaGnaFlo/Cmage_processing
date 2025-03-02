#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "server.h"

#define IMAGES_PATH "../images/"
#define INDEX "../front/index.html"
#define SCRIPTS "../front/scripts.js"
#define MIME_HTML "text/html"
#define MIME_SCRIPT "application/javascript"
#define MIME_PNG "image/png"
#define ERROR_PAGE "<html><body>An internal server error has occurred!</body></html>"


static
enum MHD_Result
send_response(
    struct MHD_Connection *connection,
    off_t size, 
    char *buffer,
    enum MHD_ResponseMemoryMode mem_mode,
    const char *content_type,
    int status_code, 
    struct MHD_Response *(*fct)(size_t, void *, enum MHD_ResponseMemoryMode)
    )
{
    int ret;
    struct MHD_Response *response = fct(size, buffer, mem_mode);
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
        response = MHD_create_response_from_buffer(strlen(ERROR_PAGE), (void*)ERROR_PAGE, MHD_RESPMEM_PERSISTENT);
        if (response) {
            ret = MHD_queue_response(connection, MHD_HTTP_INTERNAL_SERVER_ERROR, response);
            MHD_destroy_response(response);
            return ret;
        } else {
            return MHD_NO;
        }
    }

    char *buffer = (char*)malloc(sbuf.st_size);
    read(fd, buffer, sbuf.st_size);
    close(fd);

    ret = send_response(connection, sbuf.st_size, buffer, MHD_RESPMEM_MUST_FREE,
                        MIME_HTML, MHD_HTTP_OK, &MHD_create_response_from_buffer);
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
        response = MHD_create_response_from_buffer(strlen(ERROR_PAGE), (void*)ERROR_PAGE, MHD_RESPMEM_PERSISTENT);
        if (response) {
            ret = MHD_queue_response(connection, MHD_HTTP_INTERNAL_SERVER_ERROR, response);
            MHD_destroy_response(response);
            return ret;
        } else {
            return MHD_NO;
        }
    }

    char *buffer = (char*)malloc(sbuf.st_size);
    read(fd, buffer, sbuf.st_size);
    close(fd);

    response = MHD_create_response_from_buffer(sbuf.st_size, buffer, MHD_RESPMEM_MUST_FREE);
    if (response) {
        MHD_add_response_header(response, "Content-Type", MIME_SCRIPT);
        ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        MHD_destroy_response(response);
    } else {
        ret = MHD_NO;
    }
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
    
    if (-1 == (fd = open(relative_path, O_RDONLY)) || (0 != fstat(fd, &sbuf))) {
        // error accessing file
        if (fd != -1) {
            close(fd);
        }
        response = MHD_create_response_from_buffer(strlen(ERROR_PAGE), (void*)ERROR_PAGE, MHD_RESPMEM_PERSISTENT);
        if (response) {
            ret = MHD_queue_response(connection, MHD_HTTP_INTERNAL_SERVER_ERROR, response);
            MHD_destroy_response(response);
            return ret;
        } else {
            return MHD_NO;
        }
    }
    
    response = MHD_create_response_from_fd_at_offset64(sbuf.st_size, fd, 0);
    if (response) {
        MHD_add_response_header(response, "Content-Type", MIME_PNG);
        ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        MHD_destroy_response(response);
    } else {
        ret = MHD_NO;
    }
    return ret;
}

static
enum MHD_Result
answer_to_unknown(struct MHD_Connection *connection)
{
    int ret;
    const char *empty = "";
    struct MHD_Response *response;
    response = MHD_create_response_from_buffer(0, (void*)empty, MHD_RESPMEM_PERSISTENT);
    if (response) {
        ret = MHD_queue_response(connection, MHD_HTTP_NO_CONTENT, response);
        MHD_destroy_response(response);
    } else {
        ret = MHD_NO;
    }
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
    