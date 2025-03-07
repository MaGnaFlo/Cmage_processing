#include <stdbool.h>

typedef struct Image Image;

/// @brief Converts an image to gray (one channel)
/// @param dest Gray image (uninitialized)
/// @param src Original image
/// @return true if conversion ok
extern bool rgb_to_gray(Image *dest, Image *src);