#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <string.h>
#include "image.h"
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

    int width, height, channels;
    if (strcmp(properties[0], "P2") == 0 || strcmp(properties[0], "P5") == 0) {
        channels = 1;
    } else if (strcmp(properties[0], "P3") == 0 || strcmp(properties[0], "P6") == 0) {
        channels = 3;
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
    create_image(image, properties[0], width, height, channels);

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
    if (strcmp(image->type, "P2") == 0 || strcmp(image->type, "P5") == 0) {
        strcpy(extension, ".pgm");
    } else if (strcmp(image->type, "P3") == 0 || strcmp(image->type, "P6") == 0) {
        strcpy(extension, ".ppm");
    } else {
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
    unsigned char *uchar_content = malloc(image->channels * image->width * image->height);
    for (size_t i = 0; i<image->channels * image->width * image->height; ++i) {
        uchar_content[i] = (unsigned char)(255 * image->content[i]);
    }
    fwrite(uchar_content, sizeof(unsigned char), image->channels * image->width * image->height, file);
    free(uchar_content);

    // clear
    free(extension);   
    free(path);
    fclose(file);
    return true;
}

void create_image(Image *image, char *type, int width, int height, int channels)
{
    image->type = strdup(type);
    image->width = width;
    image->height = height;
    image->channels = channels;
    image->content = (double *)malloc(width * height * channels * sizeof(double));
}

void free_image(Image *image)
{
    free(image->type);
    free(image->content);
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
    if (strcmp(image->type, "P2") == 0 || strcmp(image->type, "P5") == 0) {
        color_type = PNG_COLOR_TYPE_GRAY;
    } else if (strcmp(image->type, "P3") == 0 || strcmp(image->type, "P6") == 0) {
        color_type = PNG_COLOR_TYPE_RGB;
    } else {
        fprintf(stderr, "Image type '%s' not recognized.\n", image->type);
    }
    png_set_IHDR(png_ptr, info_ptr, image->width, image->height, 8, 
                 color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png_ptr, info_ptr);

    // write image data (convert in uchar first)
    unsigned char *uchar_content = malloc(image->channels * image->width * image->height);
    for (size_t i = 0; i<image->channels * image->width * image->height; ++i) {
        uchar_content[i] = (unsigned char)(255 * image->content[i]);
    }
    png_bytep row = (png_bytep)malloc(image->channels * image->width);
    for (int j = 0; j<image->height; ++j) {
        memcpy(row, uchar_content + j * image->width * image->channels, image->channels * image->width);
        png_write_row(png_ptr, row);
    }
    png_write_end(png_ptr, info_ptr);

    // clear
    free(uchar_content);
    free(row);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(png);

    return true;
}

void print_image(Image *image)
{
    printf("%s, %d, %d, %d\n", image->type, image->width, image->height, image->channels);
    for (int i = 0; i<10; ++i) printf("%f ", image->content[i]);
    printf("\n");
}