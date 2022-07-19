#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "common.h"

//TODO: should probably do some error checking...

typedef struct Matrix {
    int rows;
    int columns;
    float* values;
} Matrix;

Matrix* matrix_new(int rows, int columns, ...) {
    Matrix* mat = (Matrix*)malloc(sizeof(Matrix));
    mat->rows = rows;
    mat->columns = columns;
    mat->values = (float*)malloc(sizeof(float) * rows * columns);

    va_list va;
    columns *= rows; // now columns = number of items in matrix, because explicitly stating that is lame 
    va_start(va, columns);
    for (int i = 0; i < columns; i++) {
        mat->values[i] = (float)va_arg(va, double);
    }
    va_end(va);

    return mat;
}

// for when we've already malloced the array values
Matrix* matrix_new_direct(int rows, int columns, float* values) {
    Matrix* mat = (Matrix*)malloc(sizeof(Matrix));
    mat->rows = rows;
    mat->columns = columns;
    mat->values = values;
    return mat;
}

void matrix_free(Matrix* mat) {
    free(mat->values);
    free(mat);
}

void matrix_print(Matrix* mat) {
    for (int i = 0; i < mat->rows * mat->columns; i++) {
        if (i % mat->columns == 0) {
            printf("\n");
        }
        printf("%f ", mat->values[i]);
    }
    printf("\n");
}

Matrix* matrix_multiply(Matrix* mat0, Matrix* mat1) {
    // row0 by column0 * row1 by column1
    // addition pairs = column0
    // size of new matrix = row 0 * column1
    ASSERT(mat0->columns == mat1->rows, "Invalid matrix multiplication!"); 

    // VISUALISE 2 * 3 as WELL
    // 3*2   2*3      3*3
    // 0 1   0 1 2    0 1 2
    // 2 3 * 3 4 5  = 3 4 5
    // 4 5            6 7 8 
    //
    // 3*2   2*2    3*2
    // 0 1   0 1    0 1
    // 2 3 * 2 3  = 2 3
    // 4 5          4 5

    int matSize = mat0->rows * mat1->columns;
    float* values = (float*)malloc(sizeof(float) * matSize);
    int lineLength = max(mat0->rows, mat1->rows); // this = max(mat 0 row, mat 1 row) 
    for (int i = 0; i < matSize; i++) {
        float value = 0;
        for (int j = 0; j < mat0->columns; j++) {
            // multiply rows of mat0 by columns of mat1
            // (i / lineLength) -> row of mat0 | column of mat1
            // printf("mat0: index: %d\n", (i / lineLength) * mat0->rows + j]);
            // printf("mat1: index: %d\n", (i / lineLength) + mat0->rows * j);
            value += mat0->values[(i / lineLength) * mat0->rows + j] * mat1->values[(i / lineLength) + mat0->rows * j];
        }
        values[i] = value;
    }
    return matrix_new_direct(mat0->rows, mat1->columns, values);
}

// operates directly on mat0 if both matrices are square and of the same dimensions 
void matrix_multiply_inplace(Matrix* mat0, Matrix* mat1) {
    ASSERT((mat0->rows == mat0->columns) && (mat1->rows == mat1->columns), "Square matrices not provided!");
    ASSERT(mat0->rows == mat1->rows, "Matrices of same dimension not provided!");
}

 
void test() {
    Matrix* m0 = matrix_new(1, 3,   1., 2., 3.);
    Matrix* m1 = matrix_new(3, 1,   4., 5., 6.);



    // matrix_multiply(m0, m1);
    Matrix* res = matrix_multiply(m0, m1);
    matrix_print(res);

    matrix_free(m0);
    matrix_free(m1);
}






#endif
