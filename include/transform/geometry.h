#include <stdbool.h>

typedef struct Image Image;

typedef enum {
    INTERP_NEAREST,
    INTERP_BILINEAR
} INTERP;

/// @brief Flip the image horizontally
/// @param dest Flipped image (uninitialized)
/// @param src Original image
/// @return bool if transform ok
extern bool flip_horizontal(Image *dest, Image *src);

/// @brief Flip the image vertically
/// @param dest Flipped image (uninitialized)
/// @param src Original image
/// @return bool if transform ok
extern bool flip_vertical(Image *dest, Image *src);

/// @brief Resizes an image to the desired size
/// @param dest 
/// @param src 
/// @param width 
/// @param height 
/// @param interp 
/// @return 
extern bool resize(Image *dest, Image *src, int width, int height, INTERP interp);