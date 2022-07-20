set(TESTS "${PROJECT_NAME}_tests")
set(SOURCES
    matrix_test.h
    main_test.c
)

add_executable(${TESTS} ${SOURCES})
