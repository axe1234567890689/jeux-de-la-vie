#ifndef WINDOW_H
#define WINDOW_H
#include <SDL.h>
#include <stdbool.h>

typedef struct color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
    struct color *suivant;
} color;

typedef struct fenetre {
    SDL_Window *window;
    SDL_Renderer *renderers;
    bool fill;
} fenetre;

void InitWindow();
unsigned char NewWindow(char *titre, int x, int  y, int w, int  h);

void RenderPresent();
struct color *NewColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void ChangeColor(unsigned char index, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void ChangeColorC(unsigned char index, struct color *color);
void DrawPoint(unsigned char index, int x, int y);
void DrawLine(unsigned char index, int x1, int y1, int x2, int y2);
void DrawRectangle(unsigned char index, int x1, int y1, int x2, int y2);
void DrawCircle(unsigned char index, int centerx, int centery, int r, bool fill);
void Pause(int delay);

void CloseWindows();

#endif