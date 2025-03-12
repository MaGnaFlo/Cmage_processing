#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <string.h>
#include "image/image.h"
#include <png.h>
#include <math.h>

/// @brief Reads the next line of a FILE and removed whitespaces before and after
/// @param line 
/// @param len 
/// @param file 
/// @return Number of characters read
static ssize_t get_trimmed_line(char** line, size_t *len, FILE *file)
{
    int read = getline(line, len, file);
    if (read == -1) {
        perror("Error reading file");
        return 0;
    }

    // trim left
    char *start = *line, *end;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }

    // trim right
    end = start + strlen(*line) - 1;
    while(end > start && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }

    // finalization
    if (start != *line) {
        memmove(*line, start, end-start+2);
    }
    return read;
}

/// @brief Frees the resources after or during the loading of an image
/// @param line Current line of the read file
/// @param file Image file
/// @param properties Properties of the images, as read from the header of the image file
static void free_load_resources(char *line, FILE *file, char *properties[3])
{
    free(line);
    fclose(file);
    for (unsigned j = 0; j<3; ++j) {
        if (properties[j] != NULL) {
            free(properties[j]);
        }
    }
}

/// @brief Reads the header of the PGM/PPM file
/// @param line Pointer to line
/// @param len Pointer to the allocated size of the line
/// @param file File from which the header is read
/// @param properties Properties of the image
/// @return true if reading was successful
bool read_header(char **line, size_t *len, FILE *file, char *properties[3])
{
    ssize_t read;
    unsigned int i = 0;
    while (i<3) {
        read = get_trimmed_line(line, len, file);
        if (read == -1) {
            perror("Error reading line");
            return false;
        }
        if (*line[0] == '#') continue; // skip comments

        properties[i] = strdup(*line);
        if (properties[i] == NULL) {
            perror("Memory allocation failed");
            return false;
        }
        ++i;
    }
    return true;
}

bool load_image(Image *image, const char *path)
{
    FILE *file;
    file = fopen(path, "r");
    if (file == NULL) {
        perror("Could not open file");
        return false;
    }

    // properties
    char *line = NULL;
    size_t len = 0;
    char *properties[3] = {NULL, NULL, NULL};
    if (!read_header(&line, &len, file, properties)) {
        perror("Could not read file header");
        free_load_resources(line, file, properties);
        return false;
    }

    IMAGE_TYPE type;
    int width, height, channels;
    if (strcmp(properties[0], "P2") == 0 || strcmp(properties[0], "P5") == 0) {
        channels = 1;
        type = GRAY;
    } else if (strcmp(properties[0], "P3") == 0 || strcmp(properties[0], "P6") == 0) {
        channels = 3;
        type = RGB;
    } else {
        perror("Unknown image type");
        free_load_resources(line, file, properties);
        return false;
    }

    // width and height
    char *p;
    p = strtok(properties[1], " ");
    if (p) width = atoi(p);
    p = strtok(NULL, " ");
    if (p) height = atoi(p);

    // create image
    create_image(image, type, width, height, channels);

    // read content
    if (image->content == NULL) {
        perror("Failed to allocate memory for image content");
        free_load_resources(line, file, properties);
        return false;
    }

    unsigned char *uchar_content = malloc(width * height * channels);
    fread(uchar_content, 1, width * height * channels, file);
    for (size_t i = 0; i<width*height*channels; ++i) {
        image->content[i] = (float)uchar_content[i] / 255.0;
    }

    free_load_resources(line, file, properties);
    return true;
}

bool save_image(Image *image, const char *name)
{
    // find correct extension based on type
    char *extension = (char*)malloc(5);
    if (!extension) {
        perror("Error allocating memory for extension");
        return false;
    }
    switch (image->type) {
    case GRAY: strcpy(extension, ".pgm"); break;
    case RGB: strcpy(extension, ".ppm"); break;
    default:
        perror("Image type not recognized");
        free(extension);
        return false;
    }

    char *path = (char*)malloc(strlen(name)+strlen(extension)+1);
    if (!path) {
        perror("Error allocating memory for image path");
        free(extension);
        return false;
    }
    strcpy(path, name);
    strcat(path, extension);

    // file
    FILE *file;
    file = fopen(path, "w");
    if (file == NULL) {
        perror("Could not open file");
        free(extension);
        free(path);
        return false;
    }
    
    // data
    unsigned char *uchar_content = malloc(image->width * image->height * image->channels);
    for (size_t i = 0; i<image->width * image->height * image->channels; ++i) {
        uchar_content[i] = (unsigned char)(255 * image->content[i]);
    }
    switch (image->type) {
        case GRAY:
            fprintf(file, "%s\n", "P5");
            break;
        case RGB: [[fallthrough]];
        default:
            fprintf(file, "%s\n", "P6");
            break;
    }
    fprintf(file, "%d %d\n", image->width, image->height);
    fprintf(file, "255\n");
    fwrite(uchar_content, sizeof(unsigned char), image->width * image->height * image->channels, file);
    free(uchar_content);

    // clear
    free(extension);   
    free(path);
    fclose(file);
    return true;
}

void create_image(Image *image, IMAGE_TYPE type, int width, int height, int channels)
{
    image->type = type;
    image->width = width;
    image->height = height;
    image->channels = channels;
    image->content = (double *)malloc(width * height * channels * sizeof(double));
}

void free_image(Image *image)
{
    free(image->content);
    image = NULL;
}

bool image_to_png(Image *image, const char *png_file_path)
{
    // initialize png struct
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        perror("Error creating PNG struct");
        return false;
    }

    // initialize info struct
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        perror("Error creating PNG info struct");
        return false;
    }

    // handle possible errors
    if (setjmp(png_jmpbuf(png_ptr))) {
        perror("Error creating PNG");
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return false;
    }

    // png file
    FILE *png = fopen(png_file_path, "wb");
    if (!png) {
        perror("Error opening PNG file for writing");
        return false;
    }

    // init I/O
    png_init_io(png_ptr, png);

    // set image info
    int color_type;
    switch (image->type) {
    case GRAY:
        color_type = PNG_COLOR_TYPE_GRAY;
        break;
    case RGB:
        color_type = PNG_COLOR_TYPE_RGB;
        break;
    default:
        perror("Image type not recognized");
        break;
    }
    png_set_IHDR(png_ptr, info_ptr, image->width, image->height, 8, 
                 color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png_ptr, info_ptr);

    // write image data (convert in uchar first)
    unsigned char *uchar_content = malloc(image->width * image->height * image->channels);
    for (size_t i = 0; i<image->width * image->height * image->channels; ++i) {
        uchar_content[i] = (unsigned char)(255 * image->content[i]);
    }
    png_bytep row_data = (png_bytep)malloc(image->channels * image->width);
    for (int row = 0; row < image->height; ++row) {
        memcpy(row_data, uchar_content + row * image->width * image->channels, image->channels * image->width);
        png_write_row(png_ptr, row_data);
    }
    png_write_end(png_ptr, info_ptr);

    // clear
    free(uchar_content);
    free(row_data);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(png);

    return true;
}

void print_image(Image *image)
{
    printf("%d, %d, %d, %d\n", image->type, image->width, image->height, image->channels);
    for (int col = 0; col<2; ++col) {
        for (int row = 0; row<2; ++row) {
            double * pixel = pixel_at(image, col, row);
            double r = *pixel, g = *(pixel+1), b = *(pixel+2);
            printf("(%f %f %f) ", r, g, b);
        }
        printf("\n");
    }
    printf("\n");
}

double * pixel_at(Image *image, int col, int row)
{
    if (row < 0 || row >= image->height || col < 0 || col >= image->width) {
        fprintf(stderr, "Fetching pixel out of bounds: %d %d (for width %d and height %d)\n", col, row, image->width, image->height);
        return NULL;
    }
    double *pixel = image->content + row * image->width * image->channels
                                   + col * image->channels;
    return pixel;
}

bool copy_image(Image *dest, Image *src)
{
    create_image(dest, src->type, src->width, src->height, src->channels);
    if (!dest->content) {
        perror("Error during image allocation");
        return false;
    }
    size_t size = src->width * src->height * src->channels * sizeof(double);
    if (!memcpy(dest->content, src->content, size)) {
        perror("Error copying image content");
        return false;
    }
    return true;
}

bool add_images(Image *dest, Image *I, Image *J)
{
    if (I->width != J->width || I->height != J->height || I->channels != J->channels) {
        fprintf(stderr, "Cannot multiply images of different sizes (%dx%dx%d and %dx%dx%d)", 
                I->width, I->height, I->channels, J->width, J->height, J->channels);
        return false;
    }
    create_image(dest, I->type, I->width, I->height, I->channels);
    if (!dest->content) {
        perror("Error during image allocation");
        return false;
    }
    for (int i = 0; i < I->height; ++i) {
        for (int j = 0; j < I->width; ++j) {
            for (int c = 0; c < I->channels; ++c) {
                *(pixel_at(dest, j, i)+c) = *(pixel_at(I, j, i)+c) + *(pixel_at(J, j, i)+c);
            }
        }
    }
    return true;
}

bool multiply_images(Image *dest, Image *I, Image *J)
{
    if (I->width != J->width || I->height != J->height || I->channels != J->channels) {
        fprintf(stderr, "Cannot multiply images of different sizes (%dx%dx%d and %dx%dx%d)", 
                I->width, I->height, I->channels, J->width, J->height, J->channels);
        return false;
    }
    create_image(dest, I->type, I->width, I->height, I->channels);
    if (!dest->content) {
        perror("Error during image allocation");
        return false;
    }
    for (int i = 0; i < I->height; ++i) {
        for (int j = 0; j < I->width; ++j) {
            for (int c = 0; c < I->channels; ++c) {
                *(pixel_at(dest, j, i)+c) = *(pixel_at(I, j, i)+c) * *(pixel_at(J, j, i)+c);
            }
        }
    }
    return true;
}

bool func_image(Image *dest, Image *src, void *fct)
{
    typedef double (*F)(double);
    F fct_ = (F)fct; 
    create_image(dest, src->type, src->width, src->height, src->channels);
    if (!dest->content) {
        perror("Error during image allocation");
        return false;
    }
    for (int i = 0; i < src->height; ++i) {
        for (int j = 0; j < src->width; ++j) {
            for (int c = 0; c < src->channels; ++c) {
                *(pixel_at(dest, j, i)+c) = fct_((*(pixel_at(src, j, i)+c)));
            }
        }
    }
    return true;
}
