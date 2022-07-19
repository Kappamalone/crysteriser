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

// for when we've already malloced the matrix values 
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
    ASSERT(mat0->columns == mat1->rows, "Invalid dimensions for matrix multiplication!"); 

    int matSize = mat0->rows * mat1->columns;
    float* values = (float*)malloc(sizeof(float) * matSize);
    int resRows = mat0->rows;
    int resColumns = mat1->columns;
    int pairs = mat0->columns;
    // the row and column of the current index in the resultant matrix 
    // corresponds to the row of mat0 and the column of mat1
    // row of index             -> row of mat0
    // column of index          -> column of mat1
    // mat0 columns or mat1 rows -> pairs of terms to be added
    for (int column = 0; column < resColumns; column++) {
        for (int row = 0; row < resRows; row++) {
            // int index = column * resColumns + row;
            int index = column + row * resRows;
            // printf("index: %d\n", index);
            float value = 0;
            for (int off = 0; off < pairs; off++) {
                // clang-format off
                int mat0Index = row     * mat0->columns + off;
                int mat1Index = column  + mat1->columns * off;
                // printf("mat0: index: %d\n", mat0Index);
                // printf("mat1: index: %d\n", mat1Index);
                value += mat0->values[mat0Index] * mat1->values[mat1Index];
                // clang-format on
            }
            values[index] = value;
        }
    }

    return matrix_new_direct(mat0->rows, mat1->columns, values);
}

// operates directly on mat0 if both matrices are square and of the same dimensions 
void matrix_multiply_inplace(Matrix* mat0, Matrix* mat1) {
    ASSERT((mat0->rows == mat0->columns) && (mat1->rows == mat1->columns), "Square matrices not provided!");
    ASSERT(mat0->rows == mat1->rows, "Matrices of same dimension not provided!");

    int matSize = mat0->rows * mat1->columns;
    float* values = (float*)malloc(sizeof(float) * matSize);
    int resRows = mat0->rows;
    int resColumns = mat1->columns;
    int pairs = mat0->columns;

    for (int column = 0; column < resColumns; column++) {
        for (int row = 0; row < resRows; row++) {
            int index = column + row * resRows;
            float value = 0;
            for (int off = 0; off < pairs; off++) {
                // clang-format off
                int mat0Index = row     * mat0->columns + off;
                int mat1Index = column  + mat1->columns * off;
                value += mat0->values[mat0Index] * mat1->values[mat1Index];
                // clang-format on
            }
            values[index] = value;
        }
    }
    free(mat0->values);
    mat0->values = values;
}

 
void test() {
    /*
    Matrix* m0 = matrix_new(2, 3,   1., 2., 3.,
                                    4., 5., 6.);
    Matrix* m1 = matrix_new(3, 1,   1., 
                                    2., 
                                    3.);
    // res ->                       a
    //                              b
    */
    Matrix* m0 = matrix_new(3, 1,   1., 
                                    2., 
                                    3.);
    Matrix* m1 = matrix_new(1, 3,   1., 2., 3.);
    // res ->                       a
    //                              b



    // matrix_multiply(m0, m1);
    Matrix* res0 = matrix_multiply(m0, m1);
    // matrix_print(res0);

    Matrix* m2 = matrix_new(2, 2, 10., 20., 30., 40.);
    Matrix* m3 = matrix_new(2, 2, 50., 60., 70., 80.);
    Matrix* res1 = matrix_multiply(m2,m3);
    matrix_multiply_inplace(m2,m3);
    matrix_print(m2);
    matrix_print(res1);

    matrix_free(res0);
    matrix_free(res1);
    matrix_free(m0);
    matrix_free(m1);
    matrix_free(m2);
    matrix_free(m3);
}

#endif
