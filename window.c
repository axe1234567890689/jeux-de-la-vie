#include <stdio.h>
#include <SDL.h>
#include <stdbool.h>
#include "window.h"

static SDL_Window *windows[10] = {NULL};
static SDL_Renderer *renderers[10] = {NULL};
struct color *firstcolor = NULL;
bool fill = true;

void InitWindow() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        SDL_Log("Erreur : %s\n dans %s fichier\n à la ligne %d\n", SDL_GetError(), __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
}

void RenderPresent() {
    for (unsigned char i = 0; i < 5; i++) {
        if (windows[i] != NULL) {
            SDL_RenderPresent(renderers[i]);
            if (fill)
                SDL_RenderClear(renderers[i]);
        }
    }
}

void ChangeColor(unsigned char index, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    SDL_SetRenderDrawColor(renderers[index], r, g, b, a);
}

void ChangeColorC(unsigned char index, struct color *color) {
    if (color == NULL) {fill = false;return;}
    SDL_SetRenderDrawColor(renderers[index], color->r, color->g, color->b, color->a);
    fill = true;
}

void DrawPoint(unsigned char index, int x, int y) {
    SDL_RenderDrawPoint(renderers[index], x, y);
}

void DrawLine(unsigned char index, int x1, int y1, int x2, int y2) {
    SDL_RenderDrawLine(renderers[index], x1, y1, x2, y2);
}

void DrawRectangle(unsigned char index, int x1, int y1, int x2, int y2) {
    SDL_Rect rectangle;
    rectangle.x = x1;
    rectangle.y = y1;
    rectangle.w = x2 - x1;
    rectangle.h = y2 - y1;
    SDL_RenderDrawRect(renderers[index], &rectangle);
}

unsigned char NewWindow(char *titre, int x, int  y, int w, int  h) {
    if (x == -1) {
        x = SDL_WINDOWPOS_CENTERED;
    }
    if (y == -1) {
        y = SDL_WINDOWPOS_CENTERED;
    }

    SDL_Window *window = SDL_CreateWindow(titre, x, y, w, h, SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (window == NULL || renderer == NULL) {
        SDL_Log("Erreur : %s\n dans le fichier %s\nà la ligne %d\n", SDL_GetError(), __FILE__, __LINE__);
        CloseWindows();
        exit(EXIT_FAILURE);
    }

    for (unsigned char i = 0; i < 5; i++) {
        if (windows[i] == NULL) {
            windows[i] = window;
            renderers[i] = renderer;
            return i;
        }
    }
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    return -1;
}

void Pause(int delay) {
    SDL_Delay(delay);
}

struct color *NewColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    struct color *result = (struct color *) malloc(sizeof(struct color));
    result->r = r;
    result->g = g;
    result->b = b;
    result->a = a;
    result->suivant = NULL;
    if (firstcolor == NULL) {
        firstcolor = result;
        return result;
    }
    struct color *current = firstcolor;
    while (current->suivant != NULL) {
        current = current->suivant;
    }
    current->suivant = result;
    return result;
}

void CloseWindows() {
    for (unsigned char i = 0; i < 5; i++) {
        if (windows[i] != NULL) {
            SDL_DestroyWindow(windows[i]);
            windows[i] = NULL;
            SDL_DestroyRenderer(renderers[i]);
            renderers[i] = NULL;
        }
    }
    struct color *current = firstcolor;
    struct color *asuppr = NULL;
    while (current != NULL) {
        asuppr = current;
        current = current->suivant;
        free(asuppr);
    }

    SDL_Quit();
}

void DrawCircle(unsigned char index, int centerx, int centery, int r, bool fill) {
    int x = 0;
    int y = -r;
    while (x <= -y) {
        if (fill) {
            DrawLine(index, centerx - x, centery + y, centerx + x, centery + y);
            DrawLine(index, centerx - x, centery - y, centerx + x, centery - y);
            DrawLine(index, centerx - y, centery + x, centerx + y, centery + x);
            DrawLine(index, centerx - y, centery - x, centerx + y, centery - x);
        } else {
            DrawPoint(index, centerx + y, centery + x);
            DrawPoint(index, centerx - y, centery + x);
            DrawPoint(index, centerx + y, centery - x);
            DrawPoint(index, centerx - y, centery - x);
            DrawPoint(index, centerx + x, centery + y);
            DrawPoint(index, centerx - x, centery + y);
            DrawPoint(index, centerx + x, centery - y);
            DrawPoint(index, centerx - x, centery - y);
        }
        x ++;
        if ((x * x + (y + 1) * (y + 1)) > r * r) {
            y ++;
        }
    }
}
