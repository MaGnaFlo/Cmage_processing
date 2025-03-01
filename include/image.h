#include <stdlib.h>
#include <stdbool.h>

/// @brief Image structure
typedef struct Image {
    unsigned int width;
    unsigned int height;
    unsigned int channels;
    unsigned char *content;
} Image;

/// @brief Loads a PPM / PGM image
/// @param path Path to image
/// @param image Image struct to store data
/// @return true if loading is successful
extern bool load_image(const char *path, Image *image);