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
    int resRows = mat0->rows;
    int resColumns = mat1->columns;
    int pairs = mat0->columns;
    // the row and column of the current index in the resultant matrix 
    // corresponds to the row of mat0 and the column of mat1
    // row of index             -> row of mat0
    // column of index          -> column of mat1
    // mat0 columns or mat1 rows -> pairs of terms to be added
    for (int row = 0; row < resRows; row++) {
        for (int column = 0; column < resColumns; column++) {
            int index = column * resColumns + row;
            // int index = column + row * resRows;
            printf("index: %d\n", index);
            float value = 0;
            for (int i = 0; i < pairs; i++) {
                // clang-format off
                int mat0Index = row     * mat0->columns + i;
                int mat1Index = column  + mat1->columns * i;
                printf("mat0: index: %d\n", mat0Index);
                printf("mat1: index: %d\n", mat1Index);
                value += mat0->values[mat0Index] * mat1->values[mat1Index];
                // clang-format on
            }
            values[index] = value;
        }
    }
    /*
    int lineLength = max(mat0->rows, mat1->rows); // this = max(mat 0 row, mat 1 row) 
    printf("line length: %d\n", lineLength);
    for (int i = 0; i < matSize; i++) {
        float value = 0;
        for (int j = 0; j < mat0->columns; j++) {
            // multiply rows of mat0 by columns of mat1
            // (i / lineLength) -> row of mat0 | column of mat1
            int mat0Index = (i / lineLength) * mat0->rows       +   j;
            int mat1Index = (i / lineLength) + mat1->columns    *   j;
            printf("mat0: index: %d\n", mat0Index);
            printf("mat1: index: %d\n", mat1Index);
            printf("i value    : %d\n", i);
            // printf("\n");
            value += mat0->values[mat0Index] * mat1->values[mat1Index];
        }
        values[i] = value;
    }
    */

    return matrix_new_direct(mat0->rows, mat1->columns, values);
}

// operates directly on mat0 if both matrices are square and of the same dimensions 
void matrix_multiply_inplace(Matrix* mat0, Matrix* mat1) {
    ASSERT((mat0->rows == mat0->columns) && (mat1->rows == mat1->columns), "Square matrices not provided!");
    ASSERT(mat0->rows == mat1->rows, "Matrices of same dimension not provided!");
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
    Matrix* res = matrix_multiply(m0, m1);
    matrix_print(res);

    matrix_free(m0);
    matrix_free(m1);
}






#endif
