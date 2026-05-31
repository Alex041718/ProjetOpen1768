//===========================================================//
// timer.c : base de temps a l'aide du Timer 0 du LPC1768
//===========================================================//
// Idee generale d'un timer :
//  - Un compteur monte tout seul, au rythme de l'horloge du processeur.
//  - Le "prescaler" ralentit ce comptage (1 increment toutes les X microsecondes).
//  - Quand le compteur atteint une valeur "match", il declenche une interruption
//    (et on peut le remettre a zero pour recommencer) -> ca cree une base de temps periodique.
//===========================================================//

#include "lpc17xx_timer.h"    // bibliotheque de gestion des timers
#include "lpc17xx_pinsel.h"   // configuration du role des broches

//-----------------------------------------------------------//
// InitTimer : configure le Timer 0 pour declencher une interruption
//             reguliere toutes les 10 ms.
//   1) configure une broche en sortie "match" (visualisation, optionnel)
//   2) regle le prescaler (vitesse de comptage)
//   3) regle la valeur de match (quand declencher) + remise a zero
//   4) demarre le timer et autorise son interruption
//-----------------------------------------------------------//
void InitTimer() {
	TIM_TIMERCFG_Type maconfigtimer;      // structure de config generale du timer (prescaler...)
	TIM_MATCHCFG_Type monmatch;           // structure de config du "match" (seuil de declenchement)


	PINSEL_CFG_Type maconfig ;            // config d'une broche
	maconfig.Portnum = 1;                 // port 1 (broches P1.x)
	maconfig.Pinnum = 28;                 // broche P1.28
	maconfig.Funcnum = 3;                 // fonction 3 de P1.28 = sortie MAT0.0 (sortie "match" du Timer0)
	maconfig.Pinmode = PINSEL_PINMODE_PULLUP;     // resistance de tirage vers le haut
	maconfig.OpenDrain = PINSEL_PINMODE_NORMAL;   // sortie normale
	PINSEL_ConfigPin(& maconfig);         // applique la config a P1.28




	maconfigtimer.PrescaleOption = TIM_PRESCALE_USVAL;  // on exprime le prescaler en microsecondes
	maconfigtimer.PrescaleValue = 100;    // le compteur s'incremente toutes les 100 us

	TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &maconfigtimer); // initialise le Timer0 en mode "compteur de temps"

	monmatch.MatchChannel = 0;            // on utilise le canal de comparaison numero 0 (MR0)
	monmatch.MatchValue = 100;            // declenche quand le compteur atteint 100
	                                      // 100 ticks x 100 us = 10 000 us = 10 ms -> interruption toutes les 10 ms
	monmatch.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;  // bascule la broche MAT0.0 a chaque match (utile a l'oscillo)
	monmatch.ResetOnMatch = ENABLE;       // remet le compteur a 0 au match -> comptage periodique

	monmatch.StopOnMatch = DISABLE;       // NE PAS arreter le timer au match (il doit continuer)

	monmatch.IntOnMatch = ENABLE;         // genere une interruption au match -> appelle TIMER0_IRQHandler


	TIM_ConfigMatch(LPC_TIM0, &monmatch); // applique cette config de match au Timer0

	TIM_Cmd(LPC_TIM0, ENABLE);            // demarre le Timer0 (il commence a compter)

	NVIC_EnableIRQ(TIMER0_IRQn);          // autorise le processeur a prendre en compte l'interruption du Timer0
}


//-----------------------------------------------------------//
// TIMER0_IRQHandler : fonction appelee AUTOMATIQUEMENT par le processeur
//   a chaque interruption du Timer0 (donc toutes les 10 ms ici).
//   Son nom est impose (defini dans le startup), il ne faut pas l'appeler soi-meme.
//   Pour l'instant elle ne fait rien d'utile ; c'est ici qu'on mettra plus tard
//   la base de temps (compter les 10 ms, scruter l'ecran tactile P0.19, lever un drapeau...).
//-----------------------------------------------------------//
void TIMER0_IRQHandler() {

	int i = 0;                            // (code de test, sans effet : variable locale perdue a chaque appel)

	i = i + 1;                            // a remplacer par le vrai traitement periodique a faire toutes les 10 ms

	TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT); // OBLIGATOIRE : efface le drapeau d'interruption,
	                                            // sinon le processeur croit que l'interruption est encore active
	                                            // et la rappelle sans arret.
}
