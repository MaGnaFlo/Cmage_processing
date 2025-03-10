#pragma once

/// @brief Matrix structure
typedef struct Matrix {
    unsigned int width;
    unsigned int height;
    double *data;
} Matrix;

extern Matrix zero_matrix(unsigned int height, unsigned int width);

/// @brief Creates a matrix given its width, height and data array
/// @param width Matrix width
/// @param height Matrix height
/// @param data Array of lines
/// @return Created matrix
extern Matrix create_matrix(unsigned int height, unsigned int width, double data[height][width]);

/// @brief Returns value of mat at (i,j)
/// @param mat Matrix
/// @param i Row
/// @param j Column
/// @return Value at (i,j)
extern double matrix_at(Matrix *mat, unsigned int i, unsigned int j);

/// @brief Sets matrix value
/// @param mat Matrix
/// @param i Row
/// @param j Column
/// @param val Value
extern void set_matrix_at(Matrix *mat, unsigned int i, unsigned int j, double val);

/// @brief Multiplies two matrices
/// @param A Left matrix
/// @param B Right matrix
/// @return Resulting matrix
extern Matrix matmul(Matrix *A, Matrix *B);

/// @brief Returns the transpose of a matrix
/// @param mat Original matrix
/// @return Transpose matrix
extern Matrix transpose(Matrix *mat);

/// @brief Returns the submatrix of mat, excluding row i and col j
/// @param mat Matrix
/// @param i_ex Excluded row
/// @param j_ex Excluded col
/// @return Submatrix
extern Matrix submatrix(Matrix *mat, unsigned int i_ex, unsigned int j_ex);

/// @brief Returns the comatrix (or matrix of cofactors)
/// @param mat Original matrix
/// @return Comatrix
extern Matrix comatrix(Matrix *mat);

/// @brief Returns the determinant of a matrix
/// @param mat Matrix
/// @return Determinant value
extern double determinant(Matrix *mat);

/// @brief Returns the inverse of a matrix
/// @note If the matrix can't be transpose, returns a empty matrix
/// @param mat Original matrix
/// @return Inverse matrix
extern Matrix inverse(Matrix *mat);

/// @brief Pretty-print a matrix
/// @param mat Matrix
extern void print_matrix(Matrix *mat);