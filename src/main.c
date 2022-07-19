#include "common.h"
#include "matrix.h"
#include <SDL2/SDL.h>
#include <stdio.h>

const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 720;
const int FRAMEBUFFER_LEN = SCREEN_WIDTH * SCREEN_HEIGHT;

typedef struct Rasteriser {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    uint32_t framebuffer[FRAMEBUFFER_LEN];
    uint32_t color;
} Rasteriser;

// TODO: how do singletons work again
typedef struct VertexData {
    float** vertexArrays;
    int**   vertexIndexArrays;
    float** vertexTextureArrays;
    float** vertexNormalArrays;
    int totalObjNumber;
} ObjData;

// TODO: naming convention like SDL_* for struct functions?
void set_color(Rasteriser* r, uint32_t color) { r->color = color; }

void draw_pixel(Rasteriser* r, int x, int y) {
    size_t coord = y * SCREEN_WIDTH + x;
    if (coord >= FRAMEBUFFER_LEN) {
        // printf("[WARN] Attempting to draw pixel outside of framebuffer!\n");
        return;
    }
    r->framebuffer[y * SCREEN_WIDTH + x] =
        (((r->color >> 0) & 0xff) << 24) | (((r->color >> 8) & 0xff) << 16) |
        (((r->color >> 16) & 0xff) << 8) | (((r->color >> 24) & 0xff) << 0);
}

void draw_line(Rasteriser* r, int x0, int y0, int x1, int y1) {
    // We'll be using Bresenham's line drawing algorithm here. Should probably
    // look into line clipping Taken from:
    // http://members.chello.at/~easyfilter/bresenham.html
    int dx = abs(x1 - x0);
    int dy = -abs(y1 - y0);
    int sx =
        x0 < x1 ? 1
                : -1; // Are we moving forwards or backwards in the x direction?
    int sy = y0 < y1 ? 1 : -1; // Are we moving up or down in the y direction?
    int err = dx + dy;
    int e2; /* error value e_xy */

    while (1) {
        draw_pixel(r, x0, y0);

        if (x0 == x1 && y0 == y1) {
            break; // exit if we've reached the final pixel
        }
        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx; /* e_xy+e_x > 0 */
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy; /* e_xy+e_y < 0 */
        }
    }
}

void draw_triangle(Rasteriser* r, int x0, int y0, int x1, int y1, int x2,
                   int y2) {
    draw_line(r, x0, y0, x1, y1);
    draw_line(r, x1, y1, x2, y2);
    draw_line(r, x2, y2, x0, y0);
}

void _draw_filled_triangle(Rasteriser* r, int x0, int y0, int x1, int y1,
                           int x2, int y2, int drawTop) {
    // clang-format off
    // 1) Draw the line V0V1 using the bresenham algorithm, but stop if the
    // algorithm moves one pixel in y-direction. 
    // 2) Draw also the line V0V1 using the bresenham algorithm, and stop 
    // if the algorithm moves one pixel in y-direction. 
    // 3) At this point we are on the same y-coordinate for line V0V1 as well 
    // as for line V0V2. 
    // 4) Draw the horizontal lines between both current line points. 
    // 5) Repeat above steps until you triangle is completely rasterised.
    // clang-format on

    // V0 -> V1
    int x00 = x0;
    int y00 = y0;
    int dx0 = abs(x1 - x0);
    int dy0 = -abs(y1 - y0);
    int sx0 = x0 < x1 ? 1 : -1;
    int sy0 = y0 < y1 ? 1 : -1;
    int err0 = dx0 + dy0;
    int e20; /* error value e_xy */

    // V0 -> V2
    int x01 = x0;
    int y01 = y0;
    int dx1 = abs(x2 - x0);
    int dy1 = -abs(y2 - y0);
    int sx1 = x0 < x2 ? 1 : -1;
    int sy1 = y0 < y2 ? 1 : -1;
    int err1 = dx1 + dy1;
    int e21; /* error value e_xy */

    int ychange0 = 0;
    int ychange1 = 0;
    while (1) {
        if (!ychange0) {
            // x00, y00, x1, y1
            if (x00 == x1 && y00 == y1) {
                if (drawTop) {
                    _draw_filled_triangle(r, x2, y2, x1, y1, x01, y1, 0);
                }
                break;
            }
            e20 = 2 * err0;
            if (e20 >= dy0) {
                err0 += dy0;
                x00 += sx0;
                draw_pixel(r, x00, y00);
            }
            if (e20 <= dx0) {
                err0 += dx0;
                y00 += sy0;
                ychange0 = 1;
            }
        }

        if (!ychange1) {
            // x01, y01, x2, y2
            if (x01 == x2 && y01 == y2) {
                if (drawTop) {
                    _draw_filled_triangle(r, x2, y2, x1, y1, x01, y1, 0);
                }
                break;
            }
            e21 = 2 * err1;
            if (e21 >= dy1) {
                err1 += dy1;
                x01 += sx1;
                draw_pixel(r, x01, y01);
            }
            if (e21 <= dx1) {
                err1 += dx1;
                y01 += sy1;
                ychange1 = 1;
            }
        }

        if (ychange0 && ychange1) {
            ASSERT(y00 == y01, "Filling in triangles have never been this difficult");
            int startx, endx;
            if (x00 <= x01) {
                startx = x00;
                endx = x01;
            } else {
                startx = x01;
                endx = x00;
            }
            for (; startx <= endx; startx++) {
                draw_pixel(r, startx, y00);
            }
            ychange0 = 0;
            ychange1 = 0;
        }
    }
}

void draw_filled_triangle(Rasteriser* r, int x0, int y0, int x1, int y1, int x2,
                          int y2) {
    // From:
    // http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html

    // Sort 3 points in descending using their y coords
    int temp;
    if (y0 < y1) {
        temp = y0;
        y0 = y1;
        y1 = temp;
        temp = x0;
        x0 = x1;
        x1 = temp;
    }
    if (y1 < y2) {
        temp = y1;
        y1 = y2;
        y2 = temp;
        temp = x1;
        x1 = x2;
        x2 = temp;
    }
    if (y0 < y1) {
        temp = y0;
        y0 = y1;
        y1 = temp;
        temp = x0;
        x0 = x1;
        x1 = temp;
    }
    // printf("%d %d %d\n", x0, x1, x2);
    // printf("%d %d %d\n", y0, y1, y2);

    if (y1 == y2) {
        // Bottom tri (V1 == V2)
        _draw_filled_triangle(r, x0, y0, x1, y1, x2, y2, 0);
    } else if (y0 == y1) {
        // Top tri (V0 == V1)
        _draw_filled_triangle(r, x2, y2, x1, y1, x0, y0, 0);
    } else {
        _draw_filled_triangle(r, x0, y0, x1, y1, x2, y2, 1);
    }
}

int main(int argc, char* argv[]) {
    // Initial SDL and Rasteriser setup
    Rasteriser rasteriser;
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

    // Primitive obj loading
    FILE* fp;
    char buffer[1024];
    int line = 0;
    float vertexArray[16384 * 4]; // 4 coords per vertice
    fp = fopen(argv[1], "r");
    // fp = fopen("./models/Ansem_and_Guardian/Ansem_and_Guardian.obj", "r");
    // fp = fopen("./models/level/di00_01.obj", "r");
    if (fp == NULL) {
        return 2; // TODO: proper return codes
    }

    char ignore;
    float v0, v1, v2;
    float v3 = 1.; // if not specified, w is 1. by default
    int i0, i1, i2;
    char* fstring;

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if (!strncmp(buffer, "v ", 2)) {
            sscanf(buffer, "%c %f %f %f %f", &ignore, &v0, &v1, &v2, &v3);
            vertexArray[line * 4] = v0;
            vertexArray[line * 4 + 1] = v1;
            vertexArray[line * 4 + 2] = v2;
            vertexArray[line * 4 + 3] = v3;
            ++line;
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

            // TODO: does the / frequency stay constant throughout a file? If so
            // we can find slash frequency outside of this while loop
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

            float c0 = 1.;
            float c1 = 2.;
            // TODO: perspective projection, camera, and matrix library
            int x0 = (vertexArray[(i0 - 1) * 4] + c0) * SCREEN_WIDTH / c1;
            int y0 = (vertexArray[(i0 - 1) * 4 + 1] + c0) * SCREEN_HEIGHT / c1;
            int x1 = (vertexArray[(i1 - 1) * 4] + c0) * SCREEN_WIDTH / c1;
            int y1 = (vertexArray[(i1 - 1) * 4 + 1] + c0) * SCREEN_HEIGHT / c1;
            int x2 = (vertexArray[(i2 - 1) * 4] + c0) * SCREEN_WIDTH / c1;
            int y2 = (vertexArray[(i2 - 1) * 4 + 1] + c0) * SCREEN_HEIGHT / c1;
            // set_color(&rasteriser, (rand_range(0, 0xffffff) << 8) | 0xff);
            draw_triangle(&rasteriser, x0, y0, x1, y1, x2, y2);
        }
    }
    fclose(fp);

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
    test();
    return 0;
}
