#include <stdio.h>
#include "../src/matrix.h"

int main() {
    Matrix* m = matrix_new(1,1,10.0);
    matrix_print(m);
    printf("yay!\n");
    printf("Running tests! ---- \n");

}
