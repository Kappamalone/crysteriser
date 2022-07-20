set(TESTS "${PROJECT_NAME}_tests")
set(SOURCES
    test_matrix.c
    test_main.c
)

add_executable(${TESTS} ${SOURCES})
