#pragma once
#include <stdbool.h>

typedef struct Matrix Matrix;
typedef struct Image Image;

/// @brief Convolves an image with a kernel
/// @param dest Filtered image (uninitialized)
/// @param src Source image
/// @param kernel Matrix filter
/// @return bool if filtering ok
extern bool filter2D(Image *dest, Image *src, Matrix *kernel);