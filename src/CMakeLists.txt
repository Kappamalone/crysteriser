set(SOURCES
    main.c
    rasteriser.h
    vertexdata.h
    matrix.h
    common.h
)

find_package(SDL2)
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE m)
