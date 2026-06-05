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

	  // Init(); // init variables globales et pinsel pour IT => a faire

	  lcd_Initializtion();              // initialise les broches de l'ecran ET l'ecran LCD (a faire 1 seule fois)

		// --- petit exemple d'affichage : un texte et quelques carres de couleur ---
	  n=sprintf(chaine,"Mon super texte      ");          // prepare le texte dans la variable globale "chaine"
	  LCD_write_english_string (32,30,chaine,White,Blue); // affiche le texte (blanc sur fond bleu) en (32,30)
	  //dessiner_rect(10,60,110,110,2,1,Black,Yellow);      // carre jaune, bord noir
	  //dessiner_rect(120,60,110,110,2,1,Black,Green);      // carre vert
	  //dessiner_rect(10,170,110,110,2,1,Black,Blue);       // carre bleu
	  //dessiner_rect(120,170,110,110,2,1,Black,Red);     // carre rouge (desactive)

	  touch_init();                     // initialise la dalle tactile (a laisser seulement si on utilise le tactile)

        InitMemoire();                    // prepare le bus I2C0 et la memoire (broches + controleur)
        InitTimer();                      // demarre la base de temps (interruption toutes les 10 ms)

        EcritureMemoire(2000,20);         // test : ecrit la valeur 20 a l'adresse 2000 de la memoire

    while(1) {

        // voir pour déplacer des carré tel des obstacles


        // --- tactile : dessine un carre rouge la ou on touche l'ecran ---
        if (flagTouch)
        {
            int xaff, yaff;                 // coordonnees en pixels (declarations EN PREMIER, regle C90)

            flagTouch = 0;                  // on consomme le drapeau
            touch_read();                   // remplit les variables globales touch_x / touch_y

            // mise a l'echelle : repere tactile (brut) -> repere affichage (pixels)
            // sur notre ecran, les deux axes vont dans le meme sens (pas d'inversion)
            xaff = (touch_x - XT_MIN) * ECRAN_L / (XT_MAX - XT_MIN);
            yaff = (touch_y - YT_MIN) * ECRAN_H / (YT_MAX - YT_MIN);

            // recentrer le carre sur le point touche (sinon c'est son coin qui suit le doigt)
            xaff = xaff - TAILLE/2;
            yaff = yaff - TAILLE/2;

            // rester dans l'ecran : on empeche le carre de deborder
            if (xaff < 0) xaff = 0;
            if (yaff < 0) yaff = 0;
            if (xaff > ECRAN_L - TAILLE) xaff = ECRAN_L - TAILLE;
            if (yaff > ECRAN_H - TAILLE) yaff = ECRAN_H - TAILLE;

            lcd_clear(White);                                            // efface l'ancien dessin
            dessiner_rect(xaff, yaff, TAILLE, TAILLE, 2, 1, Black, Red); // carre rouge a l'endroit touche
        }

    }  ;                         // boucle infinie : le programme ne se termine jamais
	// pour l'instant, le main fait juste quelques inits ... a vous d'ecrire le reste

	}

//---------------------------------------------------------------------------------------------
#ifdef  DEBUG
// Appelee par la bibliotheque si une verification d'argument echoue (mode DEBUG) : on bloque ici.
void check_failed(uint8_t *file, uint32_t line) {while(1);}
#endif
