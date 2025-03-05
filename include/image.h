#include <stdbool.h>

/// @brief Image structure
typedef struct Image {
    char *type;
    unsigned int width;
    unsigned int height;
    unsigned int channels;
    double *content;
} Image;

/// @brief Printing for debug
/// @param image 
extern void print_image(Image *image);

/// @brief Loads a PPM / PGM image
/// @param path Path to image
/// @param image Image struct to store data
/// @return true if loading is successful
extern bool load_image(Image *image, const char *path);

/// @brief Saves a PPM / PGM image
/// @note Deduce the extension based on type
/// @param name Name of the image
/// @param image Image struct
/// @return true if save ok
extern bool save_image(Image *image, const char *name);

/// @brief Allocates required memory given image properties
/// @param image Pointer to image
/// @param type Image type (P2, P3, P5, P6)
/// @param width 
/// @param height 
/// @param channels 
/// @param max 
extern void create_image(Image *image, char *type, int width, int height, int channels);

/// @brief Frees image data
/// @param image  
extern void free_image(Image *image);

/// @brief Converts an image to png and saves it
/// @param image Image struct
/// @param png_file_path Resulting image file path
/// @return true if conversion and save ok
extern bool image_to_png(Image *image, const char *png_file_path);
