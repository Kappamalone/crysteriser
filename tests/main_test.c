#include <stdio.h>
#include "matrix_test.h"

#define TEST(x) \
    x##_test()

int main() {
    printf(" ---- Running tests! ---- \n");
    TEST(matrix);
    printf(" ---- Finishd tests! ---- \n");
}
