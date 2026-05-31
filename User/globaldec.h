#include "constantes.h" // fichier contenant toutes les constantes du projet
#include <stdint.h>
#include "lpc17xx_i2c.h"

// mettez ici toutes les d�clarations de variables globales
// c'est ce fichier qui fait la vraie allocation des variables

char chaine[30]; // buffer pour l'affichage sur le LCD
uint16_t touch_x, touch_y ;

// FLAG
volatile int flagColor;
volatile int flagTouch;


volatile int compteur10ms ;



