#include "constantes.h" // fichier contenant toutes les constantes du projet
#include <stdint.h>


// mettez ici toutes les "extern" correspondant aux d�clarations contenues dans globadec.h

extern char chaine[30]; // buffer pour l'affichage sur le LCD
extern uint16_t touch_x, touch_y ;

extern volatile int flagColor; 
extern volatile int flagTouch;

extern volatile int compteur10ms ;
