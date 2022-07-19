#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <assert.h>

#ifndef NDEBUG
#define ASSERT(c, m) \
do { \
    if (!(c)) { \
        fprintf(stderr, __FILE__ ":%d: assertion %s failed: %s\n", \
                        __LINE__, #c, m); \
        exit(1); \
    } \
} while(0)
#else
#define ASSERT(c, m)
#endif

int rand_range(int lower, int upper) { return rand() % (upper + 1 - lower) + lower; }
int max(int a, int b) {
    if (a >= b) {
        return a;
    }
    return b;
}

#endif
