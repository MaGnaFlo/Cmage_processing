#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <string.h>
#include "image.h"

#define P2 "P2"
#define P3 "P3"
#define P5 "P5"
#define P6 "P6"
#define BINARY  0x1
#define ASCII   0x2

/// @brief Reads the next line of a FILE and removed whitespaces before and after
/// @param line 
/// @param len 
/// @param file 
/// @return Number of characters read
static ssize_t get_trimmed_line(char** line, size_t *len, FILE *file)
{
    int read = getline(line, len, file);
    if (read == -1) {
        perror("Error reading file.");
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
            perror("Error reading line.");
            return false;
        }
        if (*line[0] == '#') continue; // skip comments

        properties[i] = strdup(*line);
        if (properties[i] == NULL) {
            perror("Memory allocation failed.");
            return false;
        }
        ++i;
    }
    return true;
}

bool load_image(const char *path, Image *image)
{
    FILE *file;
    file = fopen(path, "r");
    if (file == NULL) {
        perror("Could not open file.");
        return false;
    }

    char *line = NULL;
    size_t len = 0;
    char *properties[3] = {NULL, NULL, NULL};
    if (!read_header(&line, &len, file, properties)) {
        perror("Could not read file header.");
        free_load_resources(line, file, properties);
        return false;
    }

    // type and channels
    int type = 0x0;
    if (strcmp(properties[0], P2) == 0) {
        type = ASCII;
        image->channels = 1;
    } else if (strcmp(properties[0], P3) == 0) {
        type = ASCII;
        image->channels = 3;
    } else if (strcmp(properties[0], P5) == 0) {
        type = BINARY;
        image->channels = 1;
    } else if (strcmp(properties[0], P6) == 0) {
        type = BINARY;
        image->channels = 3;
    } else {
        perror("Unknown image type.");
        free_load_resources(line, file, properties);
        return false;
    }

    image->max = atoi(properties[2]);

    // read width and height
    char *p;
    p = strtok(properties[1], " ");
    if (p) image->width = atoi(p);
    p = strtok(NULL, " ");
    if (p) image->height = atoi(p);

    // read content
    size_t n_bytes = image->channels * image->width * image->height;
    image->content = (unsigned char*)malloc(n_bytes);
    if (image->content == NULL) {
        perror("Failed to allocate memory for image content.");
        free_load_resources(line, file, properties);
        return false;
    }
    fread(image->content, 1, n_bytes, file);

    free_load_resources(line, file, properties);
    return true;
}

bool image_as_float(FImage *fimage, Image *image)
{
    fimage->width = image->width;
    fimage->height = image->height;
    fimage->channels = image->channels;

    unsigned int nbytes = fimage->channels * fimage->width * fimage->height;
    fimage->content = (float*)calloc(nbytes, sizeof(float));
    for (unsigned int i = 0; i<nbytes; ++i) {
        *(fimage->content + i) = (float)*(image->content + i) / (float)image->max;
    }
    return true;
}