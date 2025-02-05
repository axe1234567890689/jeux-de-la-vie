#ifndef __POS_H__
#define __POS_H__

#include <stdbool.h>
#include <winsock2.h>
#include "window.h"

typedef struct posI {
        int x;
        int y;
} posI;

typedef struct grille {
        struct grille *hautGauche;
        struct grille *haut;
        struct grille *hautDroite;
        struct grille *gauche;
        struct grille *droite;
        struct grille *basGauche;
        struct grille *bas;
        struct grille *basDroite;
        Uint32 casesAvant[32];
        Uint32 casesAprès[32];
        HANDLE thread;
        struct grille *next;
} grille;

#endif