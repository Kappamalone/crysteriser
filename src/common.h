#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <assert.h>

#define ASSERT

int rand_range(int lower, int upper) { return rand() % (upper + 1 - lower) + lower; }

#endif
