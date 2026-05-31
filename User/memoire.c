//===========================================================//
// memoire.c : gestion de la memoire FRAM FM24CL16 via le bus I2C0
//===========================================================//
// Rappel sur la memoire FM24CL16 :
//  - Elle contient 2048 cases (octets), numerotees de 0 a 2047.
//  - Chaque case stocke 1 octet (8 bits).
//  - Pour designer une case parmi 2048, il faut 11 bits d'adresse (2^11 = 2048).
//  - On ne lui parle PAS avec un bus d'adresse classique, mais avec le bus I2C
//    (2 fils : SDA = donnees, SCL = horloge). Notre LPC1768 est le "maitre",
//    la memoire est l'"esclave".
//
// L'adresse 11 bits est coupee en 2 morceaux (voir datasheet, Figure 4) :
//  - les 3 bits de poids fort  -> "page select", places DANS l'adresse esclave I2C
//  - les 8 bits de poids faible -> "word address", envoyes comme 1er octet de donnee
//===========================================================//

// #define PINSEL1 (* (unsigned long *) 0x4002C004 )
#include "lpc17xx_pinsel.h"   // fonctions pour configurer le role des broches (Pin Connect Block)
#include "lpc17xx_i2c.h"      // bibliotheque qui gere tout le protocole I2C a notre place

// MEMID = "device type" de la memoire = 1010 0000 en binaire.
// Les 4 bits de poids fort (1010) identifient une memoire FM24CL16 sur le bus I2C.
#define MEMID 0xA0

//-----------------------------------------------------------//
// InitMemoire : prepare le materiel pour dialoguer avec la memoire.
//   1) configure les broches P0.27 et P0.28 en SDA0 / SCL0 (fonction I2C0)
//   2) initialise le controleur I2C0 a une certaine vitesse
//   3) autorise les echanges I2C0
// A appeler une seule fois, au debut du programme.
//-----------------------------------------------------------//
void InitMemoire() {

	//PINSEL1 =  (1 << 24) | (1 << 22);   // ancienne methode "a la main", remplacee par la biblio

	PINSEL_CFG_Type maconfig ;            // structure qui decrit comment configurer UNE broche
	maconfig.Portnum = 0;                 // on travaille sur le port 0 (les broches P0.x)
	maconfig.Pinnum = 27;                 // broche P0.27
	maconfig.Funcnum = 1;                 // fonction 1 de cette broche = SDA0 (donnees I2C0)
	maconfig.Pinmode = PINSEL_PINMODE_PULLUP;     // resistance de tirage vers le haut activee
	maconfig.OpenDrain = PINSEL_PINMODE_NORMAL;   // mode de sortie normal

	PINSEL_ConfigPin(& maconfig);         // on applique cette config a P0.27 (le & = "adresse de")

	maconfig.Pinnum = 28;                 // on reutilise la meme structure pour la broche P0.28
	                                      // (Portnum, Funcnum... restent les memes : P0.28 fonction 1 = SCL0)
	PINSEL_ConfigPin(& maconfig);         // on applique la config a P0.28 (horloge I2C0)

	I2C_Init(LPC_I2C0, 400000) ;          // initialise le controleur I2C0 a 400 kHz (<= 500 kHz impose par le projet)
	                                      // cette fonction alimente aussi le peripherique (registre PCONP) automatiquement

	I2C_Cmd(LPC_I2C0, ENABLE);            // active le controleur I2C0 : il peut maintenant emettre/recevoir

}

//-----------------------------------------------------------//
// LectureMemoire : lit 1 octet dans la memoire a l'adresse "adresse"
//                  et range la valeur lue dans *data.
// Principe (datasheet Figure 9, "Selective Read") :
//   on ECRIT d'abord le word address (pour positionner le pointeur interne),
//   puis on RELIT la donnee a cet endroit. La biblio enchaine les deux
//   automatiquement quand tx_length > 0 ET rx_length > 0.
//-----------------------------------------------------------//
void LectureMemoire(int adresse, uint8_t *data) {

	I2C_M_SETUP_Type montype;             // structure qui decrit l'echange I2C a realiser
	uint8_t madata = adresse & 0xFF;      // word address = les 8 bits de poids faible de l'adresse

	// Adresse esclave 7 bits = 1010 + 3 bits de page.
	//  (adresse & 0x700) isole les 3 bits de page (bits 8,9,10 de l'adresse).
	//  >> 7 puis | MEMID puis >> 1 : place ces bits de page a cote de "1010"
	//  pour former l'adresse esclave sur 7 bits attendue par la biblio.
	montype.sl_addr7bit = ( MEMID | ((adresse & (0x700)) >> 7))>> 1;
	montype.tx_data = & madata;           // donnee a emettre = le word address (1 octet)
	montype.tx_length = 1;                // on emet 1 octet (le word address)
	montype.tx_count = 0;                 // compteur interne, demarre a 0

	montype.rx_data = data;               // ou ranger la donnee lue : dans le buffer "data"
	montype.rx_length = 1;                // on veut lire 1 octet
	montype.rx_count = 0;                 // compteur interne, demarre a 0
	montype.retransmissions_count = 1;    // compteur de re-essais (gestion d'erreur)
	montype.retransmissions_max = 1;      // nombre max de re-essais autorises

	// Lance l'echange en mode "polling" : le processeur attend la fin de l'echange
	// (il ne fait rien d'autre pendant ce temps, pas d'interruption ici).
	I2C_MasterTransferData(LPC_I2C0, & montype, I2C_TRANSFER_POLLING);

}

//-----------------------------------------------------------//
// EcritureMemoire : ecrit l'octet "data" dans la memoire a l'adresse "adresse".
// Principe (datasheet Figure 5, "Single Byte Write") :
//   on envoie le word address PUIS la donnee, dans la meme trame I2C.
//-----------------------------------------------------------//
void EcritureMemoire(int adresse, uint8_t data) {

	I2C_M_SETUP_Type montype;             // structure qui decrit l'echange I2C
	uint8_t tabval[2];                    // petit tableau de 2 octets a envoyer

	tabval[0] = adresse & 0xFF;           // 1er octet emis = word address (8 bits faibles de l'adresse)
	tabval[1] = data;                     // 2eme octet emis = la donnee a ecrire

	// meme calcul d'adresse esclave que pour la lecture (1010 + bits de page)
	montype.sl_addr7bit = ( MEMID | ((adresse & (0x700)) >> 7))>> 1;
	montype.tx_data = tabval;             // on emet le contenu du tableau...
	montype.tx_length = 2;                // ...soit 2 octets : word address + donnee
	montype.tx_count = 0;                 // compteur interne

	montype.rx_length = 0;                // une ecriture ne lit rien -> 0 octet a recevoir
	montype.rx_data = NULL;               // pas de buffer de reception (NULL = "rien")
	montype.rx_count = 0;                 // compteur interne

	montype.retransmissions_count = 1;    // compteur de re-essais
	montype.retransmissions_max = 1;      // nombre max de re-essais


	// Lance l'ecriture en mode polling (le processeur attend la fin de l'envoi).
	I2C_MasterTransferData(LPC_I2C0, & montype, I2C_TRANSFER_POLLING);
}
