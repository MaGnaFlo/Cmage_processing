#pragma once
#include <stdbool.h>

typedef struct Matrix Matrix;
typedef struct Image Image;

/// @brief Convolves an image with a kernel
/// @param dest Filtered image (uninitialized)
/// @param src Source image
/// @param kernel Matrix filter
/// @return bool if filtering ok
extern bool filter(Image *dest, Image *src, Matrix *kernel);

/// @brief Applie a gaussian filter to an image
/// @param dest Filtered image
/// @param src Source image
/// @param kernel_size Size of the gaussian kernel
/// @param sigma Standard deviation of the gaussian function
/// @return true if filtering ok
extern bool gaussian_filter(Image *dest, Image *src, unsigned int kernel_size, double sigma);

/// @brief Filters an image with the Sobel filters and returns magnitude and angle gradient images
/// @param grad_mag Magnitude of the resulting gradient
/// @param grad_angle Angle of the resulting gradient
/// @param src Original image
/// @return true if filtering ok
extern bool sobel_filter(Image *grad_mag, Image *grad_angle, Image *src);