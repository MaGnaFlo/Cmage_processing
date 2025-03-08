#include <stdbool.h>

typedef struct Image Image;

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