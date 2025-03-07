#include <stdbool.h>

typedef struct Image Image;

/// @brief Converts an image to gray (one channel)
/// @param dest Gray image (uninitialized)
/// @param src Original image
/// @return true if conversion ok
extern bool rgb_to_gray(Image *dest, Image *src);

/// @brief Converts an image to rgb (three channels)
/// @param dest RGB image (uninitialized)
/// @param src Original image
/// @return true if conversion ok
extern bool gray_to_rgb(Image *dest, Image *src);

/// @brief Converts an RGB image to HSV
/// @param dest HSV image (uninitialized)
/// @param src RGB image
/// @return true if conversion ok
extern bool rgb_to_hsv(Image *dest, Image *src);

/// @brief Converts an HSV image to RGB
/// @param dest RGB image (uninitialized)
/// @param src HSV image
/// @return true if conversion ok
extern bool hsv_to_rgb(Image *dest, Image *src);

/// @brief Shifts the hue of a HSV image
/// @param src HSV image
/// @param shift hue shift
extern void shift_hue(Image *src, int shift);