#ifndef COMMON_H
#define COMMON_H

#include <assert.h>
#include <stdlib.h>

#define ASSERT(c, m)                                                           \
    do {                                                                       \
        if (!(c)) {                                                            \
            fprintf(stderr, "%s, line %d: assertion %s failed: %s\n",          \
                    __FILE__, __LINE__, #c, m);                                \
            exit(1);                                                           \
        }                                                                      \
    } while (0)

#ifndef NDEBUG
#define DASSERT(c, m) ASSERT(c, m)
#define TASSERT(c, m)                                                          \
    do {                                                                       \
        if (!(c)) {                                                            \
            fprintf(stderr, "%s, line %d: assertion %s failed: %s\n",          \
                    __FILE__, __LINE__, #c, m);                                \
        }                                                                      \
    } while (0)
#else
#define DASSERT(c, m)
#endif

int rand_range(int lower, int upper) {
    return rand() % (upper + 1 - lower) + lower;
}
int max(int a, int b) { return (a > b) ? a : b; }
int min(int a, int b) { return (a > b) ? b : a; }
int char_frequency(const char* string, const char* c) {
    // TODO: const char* vs char[] vs char?
    int i = 0;
    int freq = 0;
    while (string[i] != '\0') {
        if (string[i++] == c[0]) {
            ++freq;
        }
    }
    return freq;
}

#endif
