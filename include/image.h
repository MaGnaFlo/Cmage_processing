#include <stdlib.h>
#include <stdbool.h>

/// @brief Image structure
typedef struct Image {
    char *type;
    unsigned int max;
    unsigned int width;
    unsigned int height;
    unsigned int channels;
    unsigned char *content;
} Image;

/// @brief Image structure as floating point (normalize between 0 and 1)
typedef struct FImage {
    char *type;
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

/// @brief Saves a PPM / PGM image
/// @note Deduce the extension based on type
/// @param name Name of the image
/// @param image Image struct
/// @return true if save ok
extern bool save_image(const char *name, Image *image);

/// @brief Frees image data
/// @param image  
extern void free_image(Image *image);

/// @brief Converts an Image into an FImage
/// @param fimage Float image
/// @param image Unsigned char (or uint8_t) image
/// @return true if conversion ok
extern bool to_FImage(FImage *fimage, Image *image);

/// @brief Converts an image to png and saves it
/// @param image Image struct
/// @param png_file_path Resulting image file path
/// @return true if conversion and save ok
extern bool image_to_png(Image *image, const char *png_file_path);