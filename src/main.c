#include <stdio.h>
#include "rasteriser.h"
#include "vertexdata.h"
#include "common.h"
#include "matrix.h"

int main(int argc, char* argv[]) {
    // Initial SDL and Rasteriser setup
    Rasteriser rasteriser;
    rasteriser.framebuffer =
        malloc(sizeof(uint32_t) * FRAMEBUFFER_LEN);
    rasteriser.window = SDL_CreateWindow(
        "Tiny Renderer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    rasteriser.renderer =
        SDL_CreateRenderer(rasteriser.window, -1, SDL_RENDERER_ACCELERATED);
    rasteriser.texture = SDL_CreateTexture(
        rasteriser.renderer, SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

    memset(rasteriser.framebuffer, 0x00, FRAMEBUFFER_LEN * 4);
    set_color(&rasteriser, 0xffffffff);

    VertexData* vd = vertexdata_new();
    vertexdata_load_obj(vd, "../models/african_head.obj");
    // vertexdata_load_obj(vd, "../models/teapot.obj");
    float c0 = 1.;
    float c1 = 2.;
    for (int i = 0; i < vd->objs; i++) {
        for (int j = 0; j < vd->vertexFaceLengths[i]; j++) {
            int f0 = vd->vertexFaceArrays[i][j * 3];
            int f1 = vd->vertexFaceArrays[i][j * 3 + 1];
            int f2 = vd->vertexFaceArrays[i][j * 3 + 2];
            int x0 =
                (vd->vertexArrays[i][(f0 - 1) * 4] + c0) * SCREEN_WIDTH / c1;
            int x1 =
                (vd->vertexArrays[i][(f1 - 1) * 4] + c0) * SCREEN_WIDTH / c1;
            int x2 =
                (vd->vertexArrays[i][(f2 - 1) * 4] + c0) * SCREEN_WIDTH / c1;
            int y0 = (vd->vertexArrays[i][(f0 - 1) * 4 + 1] + c0) *
                     SCREEN_WIDTH / c1;
            int y1 = (vd->vertexArrays[i][(f1 - 1) * 4 + 1] + c0) *
                     SCREEN_WIDTH / c1;
            int y2 = (vd->vertexArrays[i][(f2 - 1) * 4 + 1] + c0) *
                     SCREEN_WIDTH / c1;

            // set_color(&rasteriser, (rand_range(0, 0xffffff) << 8) | 0xff);
            draw_triangle(&rasteriser, x0, y0, x1, y1, x2, y2);
        }
    }

    // Rendering loop
    char quit = 0;
    while (!quit) {
        // Handle events on queue
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            switch (e.type) {
                case SDL_QUIT:
                    quit = 1;
                    break;
                case SDL_KEYDOWN:
                    // TODO: there are still minor errors with the filled
                    // triangle
                    /*
                    memset(rasteriser.framebuffer, 0x00, FRAMEBUFFER_LEN * 4);
                    int x0 = rand_range(0, SCREEN_WIDTH - 1);
                    int x1 = rand_range(0, SCREEN_WIDTH - 1);
                    int x2 = rand_range(0, SCREEN_WIDTH - 1);
                    int y0 = rand_range(0, SCREEN_HEIGHT- 1);
                    int y1 = rand_range(0, SCREEN_HEIGHT- 1);
                    int y2 = rand_range(0, SCREEN_HEIGHT- 1);
                    set_color(&rasteriser,0x0000ffff);
                    draw_triangle(&rasteriser, x0, y0, x1, y1, x2, y2);
                    set_color(&rasteriser, 0xff0000ff);
                    draw_filled_triangle(&rasteriser, x0, y0, x1, y1, x2, y2);
                    */
                    break;
            }
        }

        SDL_RenderClear(rasteriser.renderer);
        SDL_UpdateTexture(rasteriser.texture, NULL, rasteriser.framebuffer,
                          SCREEN_WIDTH * 4);
        // use SDL_FLIP_VERTICAL because we live in a society where 0,0 is
        // bottom left instead of top left
        SDL_RenderCopyEx(rasteriser.renderer, rasteriser.texture, NULL, NULL, 0,
                         0, SDL_FLIP_VERTICAL);
        SDL_RenderPresent(rasteriser.renderer);
    }
    return 0;
}
