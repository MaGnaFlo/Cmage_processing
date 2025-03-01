#include <stdlib.h>
#include <stdbool.h>

/// @brief Image structure
typedef struct Image {
    unsigned int max;
    unsigned int width;
    unsigned int height;
    unsigned int channels;
    unsigned char *content;
} Image;

/// @brief Image structure as floating point (normalize between 0 and 1)
typedef struct FImage {
    unsigned int width;
    unsigned int height;
    unsigned int channels;
    float *content;
} FImage;


/// @brief Loads a PPM / PGM image
/// @param path Path to image
/// @param image Image struct to store data
/// @return true if loading is successful
extern bool load_image(const char *path, Image *image);

/// @brief Converts an Image into an FImage
/// @param fimage Float image
/// @param image Unsigned char (or uint8_t) image
/// @return true if conversion ok
extern bool image_as_float(FImage *fimage, Image *image);