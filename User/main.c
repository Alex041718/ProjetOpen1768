//===========================================================//
// Projet Micro - INFO1 - ENSSAT - S2 2018                   //
//===========================================================//
// File                : Programme de depart
// Hardware Environment: Open1768
// Build Environment   : Keil uVision
//===========================================================//
// main.c : point d'entree du programme. Il enchaine les initialisations
// (ecran, tactile, memoire, timer) puis tourne dans une boucle infinie.
//===========================================================//

#include "lpc17xx_gpio.h"              // gestion des broches en entree/sortie (GPIO)
#include "lpc17xx_pinsel.h"            // configuration du role des broches (Pin Connect Block)
#include "lpc17xx_libcfg_default.h"    // configuration par defaut de la bibliotheque
#include "lpc17xx_timer.h"             // gestion des timers
#include "touch\ili_lcd_general.h"     // pilote bas niveau de l'ecran LCD
#include "touch\lcd_api.h"             // fonctions d'affichage de l'ecran LCD
#include "affichagelcd.h"              // nos fonctions d'affichage (dessiner_rect, etc.)
#include "touch\touch_panel.h"         // gestion de la dalle tactile


#include "globaldec.h"                 // declarations de TOUTES nos variables globales (chaine, touch_x...)
#include <stdio.h>                     // pour sprintf (mise en forme de texte)
#include "memoire.h"                   // nos fonctions memoire : InitMemoire, EcritureMemoire, LectureMemoire
#include "timer.h"                     // notre fonction InitTimer


//===========================================================//
// Function: Main
//   Programme principal : initialise le materiel, affiche un exemple
//   a l'ecran, teste une ecriture memoire, puis boucle indefiniment.
//===========================================================//
int main(void)
{
	int n;                            // variable pour recuperer le retour de sprintf (longueur du texte)

    uint8_t lap1;     

    int dernierAffiche = -1; // derniere valeur du chrono affichée
    
    compteur1s = 0;
    
	// Init(); // init variables globales et pinsel pour IT => a faire

	lcd_Initializtion();              // initialise les broches de l'ecran ET l'ecran LCD (a faire 1 seule fois)
    lcd_clear(White);

	touch_init();                     // initialise la dalle tactile (a laisser seulement si on utilise le tactile)

    InitMemoire();                    // prepare le bus I2C0 et la memoire (broches + controleur)
    InitTimer();                      // demarre la base de temps (interruption toutes les 10 ms)

    EcritureMemoire(2000,20);         // test : ecrit la valeur 20 a l'adresse 2000 de la memoire


    
    while(1) {

        // affichage chronometre

        if (compteur1s != dernierAffiche)
        {
            dernierAffiche = compteur1s;
            
            // construction de la chaine
            n=sprintf(chaine,"Chrono en Secondes = %d", compteur1s);
	        LCD_write_english_string(10,10,chaine,Blue,White);
            // Minutes
            n=sprintf(chaine,"Chrono en Minutes = %d", compteur1s/60);
	        LCD_write_english_string(10,40,chaine,Blue,White);
            // Chrono
            n=sprintf(chaine,"  %d : %d  ", compteur1s/60, compteur1s%60);
	        LCD_write_english_string(90,70,chaine,White,Blue);

            n=sprintf(chaine,"Touch to save the time");
	        LCD_write_english_string(35,100,chaine,White,Red);
        }

        if (flagTouch)
        {
            flagTouch = 0;
            EcritureMemoire(2000, compteur1s);
            LectureMemoire(2000, &lap1); 
            n=sprintf(chaine,"=>  %d : %d  ", lap1/60, lap1%60);
	        LCD_write_english_string(90,150,chaine,White,Green);

        }
        


    }  ;                         // boucle infinie : le programme ne se termine jamais
	// pour l'instant, le main fait juste quelques inits ... a vous d'ecrire le reste

	}

//---------------------------------------------------------------------------------------------
#ifdef  DEBUG
// Appelee par la bibliotheque si une verification d'argument echoue (mode DEBUG) : on bloque ici.
void check_failed(uint8_t *file, uint32_t line) {while(1);}
#endif
