#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <winsock2.h>
#include <windows.h>

#include "window.h"
#include "pos.h"

//cd C:\Users\axel\Desktop\c
//gcc src\jeux\jeux_de_la_vie\* -o bin\jeux_de_la_vie -I include -L lib -lmingw32 -lSDL2main -lSDL2
//printf("%d\n", __LINE__);

DWORD WINAPI step(grille *grille);

posI screenSize;
posI mousePos;
unsigned char window;
bool debug = false;
color *noir;
color *blanc;
color *gris;

int main(int argc, char **argv) {
        screenSize.x = 500; screenSize.y = 500;
        mousePos.x = 500; mousePos.y = 500;

        // gestion du temps
        float deltatime = 0;
        unsigned long lastTicks = SDL_GetTicks();
        unsigned long lastDeltaTicks = 0;

        // gestion des FPS
        bool FPSLimit = true;
        unsigned char nbFPS = 60;
        int timeForNextFrame = 0;
        int timeBetweenFrame = (1. / (float) nbFPS) * 1000;

        window = NewWindow("jeu de fous", -1, -1, (int) screenSize.x, (int) screenSize.y);
        bool Jeux = true;

        grille *firstGrille = NULL;

        color *rouge = NewColor(200, 0, 0, 255);
        gris = NewColor(100, 100, 100, 255);
        noir = NewColor(0, 0, 0, 255);
        blanc = NewColor(200, 200, 200, 255);

        InitWindow();

        while (Jeux) {
                lastDeltaTicks = SDL_GetTicks();
                lastDeltaTicks -= lastTicks;
                deltatime = lastDeltaTicks / 1000.;
                lastTicks += lastDeltaTicks;

                if (FPSLimit) {
                        timeForNextFrame += lastDeltaTicks;
                        if (timeForNextFrame >= timeBetweenFrame) {
                                ChangeColorC(window, noir);
                                RenderPresent();
                                
                                timeForNextFrame -= timeBetweenFrame;
                        }
                } else {
                        ChangeColorC(window, noir);
                        RenderPresent();
                }

                SDL_Event events;

                grille *current = firstGrille;
                while ( current != NULL) 
                {
                        current->thread = CreateThread(NULL, 0, step, current, 0, NULL);
                        current = current->next;
                }
        
                while (SDL_PollEvent(&events)) {
                        switch (events.type){
                                case SDL_QUIT:
                                        Jeux = false;
                                        break;

                                case SDL_MOUSEBUTTONDOWN:
                                        if (events.button.button == SDL_BUTTON_LEFT) {
                                        }
                                        break;

                                case SDL_MOUSEBUTTONUP:
                                        if (events.button.button == SDL_BUTTON_LEFT) {
                                        }
                                        break;

                                case SDL_MOUSEMOTION:
                                        mousePos.x = events.motion.x;
                                        mousePos.y = events.motion.y;
                                        break;

                                case SDL_KEYDOWN:
                                        if (events.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                                                debug = !debug;
                                        }
                                        break;

                                default:
                                        break;
                        }
                }
        }

        CloseWindows();
        return 0;
}

DWORD WINAPI step(grille *grille) {
        return 0;
}
