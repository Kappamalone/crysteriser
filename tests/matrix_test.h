#ifndef TEST_MATRIX_H 
#define TEST_MATRIX_H

#include <stdio.h>
#include "../src/matrix.h"

void matrix_test() {
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
