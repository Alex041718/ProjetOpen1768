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

    //uint8_t lap1_sec;    
    //uint8_t lap1_min;
    uint8_t aff_sec;  
    uint8_t aff_min;

    int dernierAffiche = -1; // derniere valeur du chrono affichée
    
    compteur1s = 0; // le compteur principal
    

	lcd_Initializtion();              // initialise les broches de l'ecran ET l'ecran LCD (a faire 1 seule fois)
    lcd_clear(White);

	touch_init();                     // initialise la dalle tactile (a laisser seulement si on utilise le tactile)

    InitMemoire();                    // prepare le bus I2C0 et la memoire (broches + controleur)

    // reprise du chrono depuis la memoire non-volatile (survit au reset)
    LectureMemoire(2001, &aff_sec);   // secondes precedemment sauvegardees
    LectureMemoire(2002, &aff_min);   // minutes precedemment sauvegardees
    compteur1s = aff_min * 60 + aff_sec;   // on reprend le chrono la ou il s'etait arrete

    InitTimer();                      // demarre la base de temps (interruption toutes les 10 ms)

    // Horloge decorative (cadran de 12 carres noirs) dans la partie superieure
    dessiner_horloge(120, 70);

    // Affichage des 3 boutons en bas de l'ecran : PAUSE | LAP | RESET (70 px chacun)
    dessiner_rect(5, 250, 70, 50, 2, 1, Black, Red);      // bouton PAUSE
    n=sprintf(chaine,"PAUSE");
    LCD_write_english_string(20,270,chaine,White,Red);

    dessiner_rect(85, 250, 70, 50, 2, 1, Black, Green);   // bouton LAP
    n=sprintf(chaine,"LAP");
    LCD_write_english_string(108,270,chaine,White,Green);

    dessiner_rect(165, 250, 70, 50, 2, 1, Black, Blue);   // bouton RESET
    n=sprintf(chaine,"RESET");
    LCD_write_english_string(180,270,chaine,White,Blue);


    
    while(1) {

        // affichage chronometre

        if (compteur1s != dernierAffiche)
        {
            dernierAffiche = compteur1s;


            // enregistrement en mémoire
            EcritureMemoire(2001, compteur1s%60); // Secondes
            EcritureMemoire(2002, compteur1s/60); // Minutes

            // on recuppère pour afficher

            LectureMemoire(2001, &aff_sec);
            LectureMemoire(2002, &aff_min);
            
        
            n=sprintf(chaine,"  %d : %d  ", aff_min, aff_sec);
	        LCD_write_english_string(85,150,chaine,White,Blue);

            // aiguille des secondes : un tour complet en 60 s
            dessiner_aiguille(120, 70, compteur1s % 60);
        }

        if (flagTouch)
        {

            // On réccupère les coordonnées :
            int xaff, yaff;                 // coordonnees en pixels (declarations EN PREMIER, regle C90)

            flagTouch = 0;                  // on consomme le drapeau
            touch_read();                   // remplit les variables globales touch_x / touch_y

            // mise a l'echelle : repere tactile (brut) -> repere affichage (pixels)
            // sur notre ecran, les deux axes vont dans le meme sens (pas d'inversion)
            xaff = (touch_x - XT_MIN) * ECRAN_L / (XT_MAX - XT_MIN);
            yaff = (touch_y - YT_MIN) * ECRAN_H / (YT_MAX - YT_MIN);
            // xaff / yaff = point exact touche (en pixels) -> on le compare aux zones des boutons

            // BOUTON PAUSE 
            if (xaff > 5 && xaff < 75 && yaff > 250 && yaff < 300)
            {
                flagPause = !flagPause; // toggle du flag pause
            }

            // BOUTON LAP 
            if (xaff > 85 && xaff < 155 && yaff > 250 && yaff < 300)
            {
                // Affichage du lap
                n=sprintf(chaine,"LAP %d : %d   ", compteur1s/60, compteur1s%60);
                LCD_write_english_string(35,200,chaine,White,Green);
            }

            // BOUTON RESET 
            if (xaff > 165 && xaff < 235 && yaff > 250 && yaff < 300)
            {
                compteur1s = 0;   // remet le chrono a zero
            }

        }
        


    }  ;                         // boucle infinie : le programme ne se termine jamais
	// pour l'instant, le main fait juste quelques inits ... a vous d'ecrire le reste

	}

//---------------------------------------------------------------------------------------------
#ifdef  DEBUG
// Appelee par la bibliotheque si une verification d'argument echoue (mode DEBUG) : on bloque ici.
void check_failed(uint8_t *file, uint32_t line) {while(1);}
#endif
