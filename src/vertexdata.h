#ifndef VERTEXDATA_H
#define VERTEXDATA_H

#include <stdlib.h>
#include "common.h"

// singleton
typedef struct VertexData {
    float** vertexArrays;
    int** vertexFaceArrays;
    float** vertexTextureArrays;
    float** vertexNormalArrays;
    int* vertexFaceLengths; // number of tri's in each vertexFaceArray
    int objs;        // number of objects that we are (attempting) to render
    int objCapacity; // used for resizing vertexArrays if we have a bunch of
                     // objects to render
} VertexData;

// https://stackoverflow.com/questions/12583908/naming-convention-for-constructors-and-destructors-in-c
VertexData* vertexdata_new() {
    static VertexData vd;
    vd.objCapacity = 4; // TODO: change this once resizing is verified to work
    vd.vertexArrays = malloc(sizeof(float*) * vd.objCapacity);
    vd.vertexFaceArrays = malloc(sizeof(int*) * vd.objCapacity);
    vd.vertexTextureArrays = malloc(sizeof(float*) * vd.objCapacity);
    vd.vertexNormalArrays = malloc(sizeof(float*) * vd.objCapacity);
    vd.vertexFaceLengths = malloc(sizeof(int) * vd.objCapacity);
    vd.objs = 0;
    return &vd;
}

void vertexdata_push(VertexData* vd, float* vertexArray, int* vertexFaceArray,
                     float* vertexTextureArray, float* vertexNormalArray,
                     int vertexFaceLength) {
    vd->vertexArrays[vd->objs] = vertexArray;
    vd->vertexFaceArrays[vd->objs] = vertexFaceArray;
    vd->vertexTextureArrays[vd->objs] = vertexTextureArray;
    vd->vertexNormalArrays[vd->objs] = vertexNormalArray;
    vd->vertexFaceLengths[vd->objs] = vertexFaceLength;
    ++vd->objs;
    if (vd->objs == vd->objCapacity) {
        // WARNING: this will cause a memory leak if realloc fails, I'm just
        // being lazy here
        vd->objCapacity *= 2;
        vd->vertexArrays = realloc(vd->vertexArrays,
                                   sizeof(vd->vertexArrays) * vd->objCapacity);
        vd->vertexFaceArrays =
            realloc(vd->vertexFaceArrays,
                    sizeof(vd->vertexFaceArrays) * vd->objCapacity);
        vd->vertexTextureArrays =
            realloc(vd->vertexTextureArrays,
                    sizeof(vd->vertexTextureArrays) * vd->objCapacity);
        vd->vertexNormalArrays =
            realloc(vd->vertexNormalArrays,
                    sizeof(vd->vertexNormalArrays) * vd->objCapacity);
        vd->vertexFaceLengths =
            realloc(vd->vertexFaceLengths,
                    sizeof(vd->vertexFaceLengths) * vd->objCapacity);
    }
}

void vertexdata_free(VertexData* vertexdata) {
    free(vertexdata->vertexArrays);
    free(vertexdata->vertexFaceArrays);
    free(vertexdata->vertexTextureArrays);
    free(vertexdata->vertexNormalArrays);
}


void vertexdata_load_obj(VertexData* vd, const char* file) {
    // v*Count used for growing array if needed, and finally shrinking once all
    // data has been read using realloc() v*Count also used to deallocate arrays
    // if 0 (eg no vertex texture data in .obj file);
    // TODO: resizing
    int capacity = 2048;
    float* vertexArray =
        malloc(sizeof(float) * capacity * 4); // x,y,z,w per vertice
    int vCount = 0;
    int* vertexFaceArray =
        malloc(sizeof(int) * capacity * 4); // x,y,z,w per vertice
    int vFCount = 0;
    float* vertexTextureArray =
        malloc(sizeof(float) * capacity * 4); // x,y,z,w per vertice
    int vTCount = 0;
    float* vertexNormalArray =
        malloc(sizeof(float) * capacity * 4); // x,y,z,w per vertice
    int vNCount = 0;

    FILE* fp;
    char buffer[1024];
    int line = 0;
    fp = fopen(file, "r");
    if (fp == NULL) {
        printf("File not found!\n");
        return;
        // return 2; // TODO: proper return codes
    }

    char ignore;
    float v0, v1, v2, v3 = 1.; // if not specified, w is 1. by default
    int i0, i1, i2;
    char* fstring;
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if (!strncmp(buffer, "v ", 2)) {
            sscanf(buffer, "%c %f %f %f %f", &ignore, &v0, &v1, &v2, &v3);
            vertexArray[vCount * 4] = v0;
            vertexArray[vCount * 4 + 1] = v1;
            vertexArray[vCount * 4 + 2] = v2;
            vertexArray[vCount * 4 + 3] = v3;
            ++vCount;
            // TODO: vertex textures, vertex normals
        } else if (!strncmp(buffer, "o ", 2)) {
            printf("%s\n", buffer);
        } else if (!strncmp(buffer, "f ", 2)) {
            // clang-format off
            // We'll use the frequency of /'s to use the corresponding format string
            // [f 1 2 3]                -> 0 /'s 
            // [f 3/1 4/2 5/3]          -> 3 /'s
            // [f 7//1 8//2 9//3]       -> 6 /'s
            // [f 6/4/1 3/5/3 7/6/5]    -> 9 /'s
            // clang-format on

            int slashFrequency = 6; // TODO: char frequency function
            switch (slashFrequency) {
                case 0:
                    fstring = "%c %d %d %d";
                    sscanf(buffer, fstring, &ignore, &i0, &i1, &i2);
                    break;
                case 3:
                    fstring = "%c %d/%d %d/%d %d/%d";
                    sscanf(buffer, fstring, &ignore, &i0, &ignore, &i1, &ignore,
                           &i2, &ignore);
                    break;
                case 6:
                    fstring = "%c %d/%d/%d %d/%d/%d %d/%d/%d";
                    sscanf(buffer, fstring, &ignore, &i0, &ignore, &ignore, &i1,
                           &ignore, &ignore, &i2, &ignore, &ignore);
                    break;
            }
            vertexFaceArray[vFCount * 3] = i0;
            vertexFaceArray[vFCount * 3 + 1] = i1;
            vertexFaceArray[vFCount * 3 + 2] = i2;
            ++vFCount;
        }
    }
    fclose(fp);

    if (!vCount || !vFCount) {
        printf("No vertexes/faces defined in %s\n", file);
        return;
    }
    if (!vTCount) {
        free(vertexTextureArray);
    }
    if (!vNCount) {
        free(vertexNormalArray);
    }

    vertexdata_push(vd, vertexArray, vertexFaceArray, vertexTextureArray,
                    vertexNormalArray, vFCount);
}


#endif
