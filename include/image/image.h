#pragma once
#include <stdbool.h>

// image types
typedef enum {
    GRAY,
    RGB,
    HSV
} IMAGE_TYPE;

/// @brief Image structure
typedef struct Image {
    // char *type;
    IMAGE_TYPE type;
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
extern void create_image(Image *image, IMAGE_TYPE type, int width, int height, int channels);

/// @brief Frees image data
/// @param image  
extern void free_image(Image *image);

/// @brief Converts an image to png and saves it
/// @param image Image struct
/// @param png_file_path Resulting image file path
/// @return true if conversion and save ok
extern bool image_to_png(Image *image, const char *png_file_path);

/// @brief Returns a pointer to the pixel at row and col
/// @param image 
/// @param row 
/// @param col 
/// @return Pointer to pixel, NULL if not found
extern double * pixel_at(Image *image, int col, int row);

/// @brief Does what you think it does
/// @param dest Copy image (uninitialized)
/// @param src Origina image
/// @return true if copy ok
extern bool copy_image(Image *dest, Image *src);

/// @brief Adds two images pixel-wise
/// @param dest Result
/// @param I Source image 1
/// @param J Source image 2
/// @return true if add ok
extern bool add_images(Image *dest, Image *I, Image *J);

/// @brief Multiplies two images pixel-wise
/// @param dest Result
/// @param I Source image 1
/// @param J Source image 2
/// @return true if multiplication ok
extern bool multiply_images(Image *dest, Image *I, Image *J);

/// @brief Divides two images pixel-wise
/// @param dest Result
/// @param I Source image 1
/// @param J Source image 2
/// @return true if division ok
extern bool divide_images(Image *dest, Image *I, Image *J);

/// @brief Applies a function to each pixel of the image
/// @param dest Resulting image
/// @param src Original image
/// @param fct Pointer to function
/// @return true if operation ok
extern bool func_image(Image *dest, Image *src, void *fct);