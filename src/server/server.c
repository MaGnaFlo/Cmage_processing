#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <microhttpd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdarg.h>
#include "server/server.h"
#include "image/image.h"
#include "transform/colors.h"
#include "transform/geometry.h"

// paths
static const char * const IMAGES_PATH = "../images/";
static const char * const INDEX = "../front/index.html";
static const char * const SCRIPTS = "../front/scripts.js";
// MIME data
static const char * const MIME_TEXT = "text/plain";
static const char * const MIME_HTML = "text/html";
static const char * const MIME_SCRIPT = "application/javascript";
static const char * const MIME_PNG = "image/png";
// connection type for response
static const char * const FROM_BUFFER = "from_buffer";
static const char * const FROM_FD = "from_fd";
// defaults and temporary
static const char * const ERROR_PAGE = "<html><body>An internal server error has occurred!</body></html>";
static const char * const TEMP_LOADED_IMG = "loaded_img.png";
// struct containing a pair of key-value for transforms
typedef struct Transform {
    const char * const key;
    void * value;
} Transform;
// array of transforms
static Transform transforms[4] = {
    {.key = "rgb2gray", .value = &rgb_to_gray},
    {.key = "gray2rgb", .value = &gray_to_rgb},
    {.key = "flip_hor", .value = &flip_horizontal},
    {.key = "flip_ver", .value = &flip_vertical}
};

/// @brief Retrieves the transform given its key
/// @param key Transform key
/// @return Pointer to function
static void * find_transform(const char * const key)
{
    if (sizeof(transforms) == 0) {
        return NULL;
    }
    void * fct = NULL;
    size_t n = sizeof(transforms) / sizeof(transforms[0]);
    for (int i = 0; i<n; ++i) {
        if (strcmp(transforms[i].key, key) == 0) {
            fct = transforms[i].value;
            break;
        }
    }
    return fct;
}

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
    if (strcmp(response_type, FROM_BUFFER) == 0 && argcount == 3) {
        size_t size = va_arg(args, size_t);
        char *buffer = va_arg(args, char *);
        enum MHD_ResponseMemoryMode mem_mode = va_arg(args, enum MHD_ResponseMemoryMode);
        response = MHD_create_response_from_buffer(size, buffer, mem_mode);
    } else if (strcmp(response_type, FROM_FD) == 0 && argcount == 3) {
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
        ret = create_response(connection, MIME_TEXT, MHD_HTTP_INTERNAL_SERVER_ERROR, FROM_BUFFER, 
                              3, strlen(ERROR_PAGE), (void*)ERROR_PAGE, MHD_RESPMEM_PERSISTENT);
    }

    char *buffer = (char*)malloc(sbuf.st_size);
    read(fd, buffer, sbuf.st_size);
    close(fd);

    ret = create_response(connection, MIME_HTML, MHD_HTTP_OK, FROM_BUFFER, 
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
        ret = create_response(connection, MIME_TEXT, MHD_HTTP_INTERNAL_SERVER_ERROR, FROM_BUFFER, 
                              3, strlen(ERROR_PAGE), (void*)ERROR_PAGE, MHD_RESPMEM_PERSISTENT);
    }

    char *buffer = (char*)malloc(sbuf.st_size);
    read(fd, buffer, sbuf.st_size);
    close(fd);

    ret = create_response(connection, MIME_HTML, MHD_HTTP_OK, FROM_BUFFER, 3, sbuf.st_size, buffer, MHD_RESPMEM_MUST_FREE);
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

    Image image;
    if (!load_image(&image, relative_path)) {
        perror("Could not properly load image");
        return MHD_NO;
    }
    // we actually have to perform a conversion since HTML is not happy with PPM/PGM
    if (!image_to_png(&image, TEMP_LOADED_IMG)) {
        perror("An error occurred during PNG conversion");
        return MHD_NO;
    }
    free_image(&image);
    
    if (-1 == (fd = open(TEMP_LOADED_IMG, O_RDONLY)) || (0 != fstat(fd, &sbuf))) {
        // error accessing file
        if (fd != -1) {
            close(fd);
        }
        ret = create_response(connection, MIME_TEXT, MHD_HTTP_INTERNAL_SERVER_ERROR, FROM_BUFFER, 
                              3, strlen(ERROR_PAGE), (void*)ERROR_PAGE, MHD_RESPMEM_PERSISTENT);
    }
    ret = create_response(connection, MIME_PNG, MHD_HTTP_OK, FROM_FD, 3, sbuf.st_size, fd, 0);
    return ret;
}

static
enum MHD_Result
answer_to_transform(struct MHD_Connection *connection, const char *url)
{
    struct MHD_Response *response;
    int fd, ret;
    struct stat sbuf;

    // parse url
    char *transform_key;
    char *url_ = strdup(url);
    char *token = strtok(url_, "/");
    char *image_name = strdup(token);
    char *image_path = (char*)malloc(strlen(IMAGES_PATH)+strlen(image_name));
    strcpy(image_path, IMAGES_PATH);
    strcat(image_path, image_name);
    int i = 1;
    while (token) {
        token = strtok(NULL, "/");
        switch (i++) {
            case 2: transform_key = strdup(token); break;
            default: break;
        }
    }

    // find the appropriate transform function
    typedef bool (*transform_fct)(Image *, Image *); // I might have to use extra args here
    void * fct = find_transform(transform_key);
    if (fct == NULL) {
        perror("Unknown transform key");
        free(url_);
        free(image_name);
        free(image_path);
        free(transform_key);
        return MHD_NO;
    }
    transform_fct t_fct = (transform_fct)fct; // cast

    // source
    Image original_image;
    if (!load_image(&original_image, image_path)) {
        perror("Could not properly load image");
        free(url_);
        free(image_name);
        free(image_path);
        free(transform_key);
        return MHD_NO;
    }

    // dest
    Image transformed_image;
    if (!t_fct(&transformed_image, &original_image)) {
        free(url_);
        free(image_name);
        free(image_path);
        free(transform_key);
        free_image(&original_image);
        return MHD_NO;
    }

    // clear
    free(url_);
    free(image_name);
    free(image_path);
    free(transform_key);

    // we actually have to perform a conversion since HTML is not happy with PPM/PGM
    if (!image_to_png(&transformed_image, TEMP_LOADED_IMG)) {
        perror("An error occurred during PNG conversion");
        return MHD_NO;
    }
    free_image(&original_image);
    free_image(&transformed_image);
    
    if (-1 == (fd = open(TEMP_LOADED_IMG, O_RDONLY)) || (0 != fstat(fd, &sbuf))) {
        // error accessing file
        if (fd != -1) {
            close(fd);
        }
        ret = create_response(connection, MIME_TEXT, MHD_HTTP_INTERNAL_SERVER_ERROR, FROM_BUFFER, 
                              3, strlen(ERROR_PAGE), (void*)ERROR_PAGE, MHD_RESPMEM_PERSISTENT);
    }
    ret = create_response(connection, MIME_PNG, MHD_HTTP_OK, FROM_FD, 3, sbuf.st_size, fd, 0);
    return ret;
}

static
enum MHD_Result
answer_to_unknown(struct MHD_Connection *connection)
{
    int ret;
    const char *empty = "";
    struct MHD_Response *response;
    ret = create_response(connection, MIME_TEXT, MHD_HTTP_INTERNAL_SERVER_ERROR, FROM_BUFFER, 
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
        } else if (strstr(url, "transform") != NULL) {
            ret = answer_to_transform(connection, url);
        } else {
            ret = answer_to_unknown(connection);
        }
        return ret;
    }
    