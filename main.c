#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <winsock2.h>
#include <windows.h>

#include "window.h"
#include "pos.h"

//cd C:\Users\axel\Desktop\c
//gcc src\jeux\jeux_de_la_vie\* -o bin\jeux_de_la_vie -lws2_32 -I include -L lib -lmingw32 -lSDL2main -lSDL2_test -lSDL2
//printf("%d\n", __LINE__);

void draw();
grille *Newgrille(int x, int y);
DWORD WINAPI step(void *param);
void modifCase(grille *g, posI *pos, Uint32 value);
bool getCase(grille *g, posI *pos);
void printGrille();
void stepVide(grille *g);
void stepFull(grille *g);

posI screenSize;
posF camPos;
unsigned char window;
bool debug = false;
float zoom = 10;
color *noir;
color *blanc;
color *gris;
grille *firstGrille = NULL;

int main(int argc, char **argv) {
        screenSize.x = 1000; screenSize.y = 1000;
        camPos.x = 0; camPos.y = 0;

        // gestion du temps
        float deltatime = 0;
        unsigned long lastTicks = SDL_GetTicks();
        unsigned long lastDeltaTicks = 0;

        // gestion des FPS
        bool FPSLimit = true;
        unsigned char nbFPS = 60;
        int timeForNextFrame = 0;
        int timeBetweenFrame = (1. / (float) nbFPS) * 1000;

        bool haut = false;
        bool bas = false;
        bool gauche = false;
        bool droite = false;

        window = NewWindow("jeu de fous", -1, -1, (int) screenSize.x, (int) screenSize.y);
        bool Jeux = true;

        int vitesse = 32;

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
                        
                                draw();
                                
                                timeForNextFrame -= timeBetweenFrame;
                        }
                } else {
                        ChangeColorC(window, noir);
                        RenderPresent();

                        draw();
                }

                SDL_Event events;

                grille *current = firstGrille;
                while ( current != NULL) 
                {
                        current->thread = CreateThread(NULL, 0, step, (void *) current, 0, NULL);
                        current = current->next;
                }

                if (gauche) {
                        camPos.x -= vitesse * deltatime;
                }
                if (droite) {
                        camPos.x += vitesse * deltatime;
                }
                if (haut) {
                        camPos.y += vitesse * deltatime;
                }
                if (bas) {
                        camPos.y -= vitesse * deltatime;
                }
        
                while (SDL_PollEvent(&events)) {
                        switch (events.type){
                                case SDL_QUIT:
                                        Jeux = false;
                                        break;

                                case SDL_MOUSEBUTTONDOWN:
                                        if (events.button.button == SDL_BUTTON_LEFT) {
                                                float bigZoom = zoom * 32;
                                                float nbCarerX = screenSize.x / bigZoom;
                                                posF decallage; decallage.x = (( (int) camPos.x) % 32 + camPos.x - (int) camPos.x) * zoom; decallage.y = (( (int) camPos.y) % 32 + camPos.y - (int) camPos.y) * zoom;
                                                int x = ((events.motion.x + decallage.x) / bigZoom) + camPos.x / 32;
                                                int y = ((events.motion.y + decallage.y) / bigZoom) + camPos.y / 32;
                                                posI posCell; posCell.x = 0; posCell.y = 0;
                                                printf("click in :\nx = %d\ny = %d\n", x, y);
                                                grille *current = firstGrille;
                                                while (current != NULL && !(current->pos.x == x && current->pos.y == y))
                                                {
                                                        current = current->next;
                                                }
                                                if (current == NULL) {
                                                        current = Newgrille(x, y);
                                                }
                                                modifCase(current, &posCell, 0b1);
                                        }
                                        break;

                                case SDL_KEYDOWN:
                                        if (events.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                                                debug = !debug;
                                        } else if (events.key.keysym.scancode == SDL_SCANCODE_LEFT) {
                                                gauche = true;
                                        } else if (events.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
                                                droite = true;
                                        } else if (events.key.keysym.scancode == SDL_SCANCODE_UP) {
                                                haut = true;
                                        } else if (events.key.keysym.scancode == SDL_SCANCODE_DOWN) {
                                                bas = true;
                                        }
                                        break;

                                case SDL_KEYUP:
                                        if (events.key.keysym.scancode == SDL_SCANCODE_LEFT) {
                                                gauche = false;
                                        } else if (events.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
                                                droite = false;
                                        } else if (events.key.keysym.scancode == SDL_SCANCODE_UP) {
                                                haut = false;
                                        } else if (events.key.keysym.scancode == SDL_SCANCODE_DOWN) {
                                                bas = false;
                                        }
                                        break;

                                default:
                                        break;
                        }
                }
        }

        grille *current = firstGrille;
        grille *suivant;
        while (current != NULL)
        {
                suivant = current->next;
                free(current);
                current = suivant;
        }

        CloseWindows();
        return 0;
}

void draw() {
        int zoomBig = zoom * 32;

        posF decallage; decallage.x = (( (int) camPos.x) % 32 + camPos.x - (int) camPos.x) * -zoom; decallage.y = (( (int) camPos.y) % 32 + camPos.y - (int) camPos.y) * -zoom;
        posI casesPos; casesPos.x = camPos.x / 32; casesPos.y = camPos.y / 32;
        posI casee; casee.x = 0; casee.y = 0;

        grille *current;

        if (zoom > 1 / 16) {
                for (int x = -1; x < (screenSize.x / (zoomBig)) + 2; x++) {
                        for (int y = -1; y < (screenSize.y / (zoomBig)) + 2; y++) {
                                ChangeColorC(window, blanc);
                                char buffer[20];
                            
                                snprintf(buffer, sizeof(buffer), "x = %d", casesPos.x + x);
                                DrawString(window, x * zoomBig + decallage.x, y * zoomBig + decallage.y, buffer);
                                snprintf(buffer, sizeof(buffer), "x = %d", casesPos.y + y);
                                DrawString(window, x * zoomBig + decallage.x, y * zoomBig + decallage.y + 10, buffer);

                                current = firstGrille;
                                while (current != NULL && !(current->pos.x == casesPos.x + x && current->pos.y == casesPos.y + y)) {
                                        current = current->next;
                                }

                                ChangeColorC(window, gris);

                                if (current != NULL && !current->vide) {
                                        ChangeColorC(window, blanc);
                                        for (int i = 0; i < 32; i++) {
                                                for (int j = 0; j < 32; j++) {
                                                        casee.x = i; casee.y = j;
                                                        if (getCase(current, &casee)) {
                                                                DrawRectangle(window, x * zoomBig + decallage.x + casee.x * zoom, y * zoomBig + decallage.y + casee.y * zoom, x * zoomBig + decallage.x + (casee.x + 1) * zoom, y * zoomBig + decallage.y + (casee.y + 1) * zoom, true);
                                                        } else {
                                                                DrawPoint(window, x * zoomBig + decallage.x + casee.x * zoom, y * zoomBig + decallage.y + casee.y * zoom);
                                                        }
                                                }
                                        }
                                
                                        ChangeColorC(window, gris);
                                } else if (current != NULL) {
                                        ChangeColorC(window, blanc);

                                        DrawRectangle(window, x * zoomBig + decallage.x, y * zoomBig + decallage.y, (x + 1) * zoomBig + decallage.x, (y + 1) * zoomBig + decallage.y, false);
                                }

                                DrawPoint(window, x * zoomBig + decallage.x, y * zoomBig + decallage.y);
                        }
                }
        }
}

grille *Newgrille(int x, int y) {

        grille *current = firstGrille;
        if (firstGrille == NULL) {
                firstGrille = (grille *) malloc(sizeof(grille));
                current = firstGrille;
        } else {
                while (current->next != NULL)
                {
                        current = current->next;
                }
                current->next = (grille *) malloc(sizeof(grille));
                current = current->next;
        }

        for (int i = 0; i < 32; i++) {
                current->casesAprès[i] = 0;
                current->casesAvant[i] = 0;
        }

        current->pos.x = x;
        current->pos.y = y;
        
        current->hautGauche= NULL;
        current->haut= NULL;
        current->hautDroite= NULL;
        current->gauche= NULL;
        current->droite= NULL;
        current->basGauche= NULL;
        current->bas= NULL;
        current->basDroite= NULL;

        current->thread= NULL;
        current->vide = true;
        current->next= NULL;

        grille *voisin = firstGrille;
        while (voisin->next != NULL)
        {
                if (x - 1 <= voisin->pos.x && voisin->pos.x <= x + 1 && y - 1 <= voisin->pos.y && voisin->pos.y <= y + 1) {
                        if (voisin->pos.x == x - 1) {
                                if (voisin->pos.y == y) {
                                        current->gauche = voisin;
                                        voisin->droite = current;
                                } else if (voisin->pos.y == y + 1) {
                                        current->basGauche = voisin;
                                        voisin->hautDroite = current;
                                } else {
                                        current->hautGauche = voisin;
                                        voisin->basDroite = current;
                                }
                        } else if (voisin->pos.x == x) {
                                if (voisin->pos.y == y - 1) {
                                        current->haut = voisin;
                                        voisin->bas = current;
                                } else {
                                        current->bas = voisin;
                                        voisin->haut = current;
                                }
                        } else {
                                if (voisin->pos.y == y) {
                                        current->droite = voisin;
                                        voisin->gauche = current;
                                } else if (voisin->pos.y == y + 1) {
                                        current->basDroite = voisin;
                                        voisin->hautGauche = current;
                                } else {
                                        current->hautDroite = voisin;
                                        voisin->basGauche = current;
                                }
                        }
                }
                voisin = voisin->next;
        }

        //printGrille();

        return current;
}

void printGrille() {
        grille *current = firstGrille;
        int nbGrille = 0;
        while (current != NULL)
        {
                printf("-------------grille %d-------------\n", nbGrille);
                printf(" - stocker en %x\n", current);
                printf(" - x = %d\n - y = %d\n", current->pos.x, current->pos.y);
                printf(" - %x     %x     %x\n", current->hautGauche, current->haut, current->hautDroite);
                printf(" - %x     %x     %x\n", current->gauche, current, current->droite);
                printf(" - %x     %x     %x\n", current->basGauche, current->bas, current->basDroite);
                printf(" - %s\n", (current->vide) ? "vide" : "plein");
                current = current->next;
                nbGrille++;
        }
        printf("\nil y a %d grilles\n", nbGrille);
}

DWORD WINAPI step(void *param) {
        grille *g = (grille *) param;
        g->thread = NULL;

        if (g->vide) {
                stepVide(g);
        } else {
                stepFull(g);
        }

        for (int i = 0; i < 32; i++) {
                if (g->casesAprès[i]) {
                        g->vide = false;
                        return 0;
                }
        }

        g->vide = true;

        return 0;
}

void stepVide(grille *g) {

}

void stepFull(grille *g) {

}

void modifCase(grille *g, posI *pos, Uint32 value) {
        if (value && g->vide) {
                g->vide = false;
                if (g->hautGauche == NULL) {
                        Newgrille(g->pos.x - 1, g->pos.y - 1);
                }
                if (g->haut == NULL) {
                        Newgrille(g->pos.x, g->pos.y - 1);
                }
                if (g->hautDroite == NULL) {
                        Newgrille(g->pos.x + 1, g->pos.y - 1);
                }
                if (g->gauche == NULL) {
                        Newgrille(g->pos.x - 1, g->pos.y);
                }
                if (g->basGauche == NULL) {
                        Newgrille(g->pos.x - 1, g->pos.y + 1);
                }
                if (g->bas == NULL) {
                        Newgrille(g->pos.x, g->pos.y + 1);
                }
                if (g->basDroite == NULL) {
                        Newgrille(g->pos.x + 1, g->pos.y + 1);
                }
                if (g->droite == NULL) {
                        Newgrille(g->pos.x + 1, g->pos.y);
                }
        }
        value = value << pos->x;
        g->casesAprès[pos->y] = g->casesAprès[pos->y] || value;
}

bool getCase(grille *g, posI *pos) {
        return (g->casesAvant[pos->y] << (-pos->x + 31)) >> 31;
}
