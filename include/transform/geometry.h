#pragma once
#include <stdbool.h>

typedef struct Image Image;
typedef struct Matrix Matrix;

typedef enum {
    INTERP_NEAREST,
    INTERP_BILINEAR
} INTERP;

/// @brief Flip the image horizontally
/// @param dest Flipped image (uninitialized)
/// @param src Original image
/// @return true if transform ok
extern bool flip_horizontal(Image *dest, Image *src);

/// @brief Flip the image vertically
/// @param dest Flipped image (uninitialized)
/// @param src Original image
/// @return true if transform ok
extern bool flip_vertical(Image *dest, Image *src);

/// @brief Resizes an image to the desired size
/// @param dest Resized image
/// @param src Original image
/// @param width Target width
/// @param height Target height
/// @param interp Interpolation technique
/// @return true if resizing ok
extern bool resize(Image *dest, Image *src, int width, int height, INTERP interp);

/// @brief Rotate an image
/// @param dest Rotated image
/// @param src Original image
/// @param angle Angle (radians)
/// @param interp Interpolation technique
/// @return true if rotation ok
extern bool rotate(Image *dest, Image *src, double angle, INTERP interp);

/// @brief Warps the image according to an affine transformation
/// @param dest Warped image
/// @param src Original image
/// @param warp_matrix Affine transform matrix
/// @param interp Interpolation
/// @return true if warp ok
extern bool warp_affine(Image *dest, Image *src, Matrix *warp_matrix, INTERP interp);
