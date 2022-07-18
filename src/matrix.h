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

void matrix_free(Matrix* mat) {
    free(mat->values);
    free(mat);
}

// operates directly on mat0
void matrix_multiply(Matrix* mat0, Matrix* mat1) {
    ASSERT(mat0->columns == mat1->rows); 
}

 
// TODO: learn about dynamic memory allocation, how it ties in with structs, and begin working on matrix library
void test() {
    Matrix* m0 = matrix_new(2, 2, 1., 2., 3., 4.);
    Matrix* m1 = matrix_new(2, 2, 1., 2., 3., 4.);
    printf("%f %f %f %f\n", m0->values[0], m0->values[1], m0->values[2], m0->values[3]);
    printf("%f %f %f %f\n", m1->values[0], m1->values[1], m1->values[2], m1->values[3]);

    matrix_multiply(m0, m1);

    matrix_free(m0);
    matrix_free(m1);
}






#endif
