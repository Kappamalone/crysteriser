#ifndef VERTEXDATA_H
#define VERTEXDATA_H

#include "common.h"
#include <cglm/vec4.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// singleton
typedef struct VertexData {
    vec4* tris;
    // ignore others for now
    int objs; // number of objects that we are (attempting) to render
    int objCapacity;
} VertexData;

// https://stackoverflow.com/questions/12583908/naming-convention-for-constructors-and-destructors-in-c
VertexData* vertexdata_new() {
    static VertexData vd;
    vd.objCapacity = 4; // TODO: change this once resizing is verified to work
    vd.objs = 0;
    return &vd;
}

void vertexdata_push(VertexData* vd) {
    // vd->tris = tris;
    ++vd->objs;
    if (vd->objs == vd->objCapacity) {
        // WARNING: this will cause a memory leak if realloc fails, I'm just
        // being lazy here
        vd->objCapacity *= 2;
        printf("resizing to %d objects!\n", vd->objCapacity);
        /*
        vd->vertexFaceLengths =
            realloc(vd->vertexFaceLengths,
                    sizeof(*vd->vertexFaceLengths) * vd->objCapacity);
        */
    }
}

void vertexdata_free(VertexData* vertexdata) {
    // free(vd->tris);
}

void vertexdata_load_obj(VertexData* vd, const char* file) {
    int vertexBufferLen = 1024 * 1024; // allocate 1mb of buffer space for each
                                       // array, then downsize at the end
    float* vertexArray = malloc(vertexBufferLen); // x,y,z,w per vertice
    // glm::vec4* tris = malloc(vertexBufferLen);
    int vertexCount = 0;
    int trisCount = 0;

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
            vertexArray[vertexCount++] = v0;
            vertexArray[vertexCount++] = v1;
            vertexArray[vertexCount++] = v2;
            vertexArray[vertexCount++] = v3;
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

            int slashFrequency = char_frequency(buffer, "/");
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
            /*
            vertexFaceArray[vFCount++] = i0;
            vertexFaceArray[vFCount++] = i1;
            vertexFaceArray[vFCount++] = i2;
            */
            ++trisCount;
        }
    }

    fclose(fp);

    if (!vertexCount || !trisCount) {
        printf("No vertexes/faces defined in %s\n", file);
        return;
    }

    free(vertexArray);
    // realloc to change size of tris;
    // vd->tris = tris;
}

#endif
