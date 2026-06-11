#include "touch/ili_lcd_general.h"
#include "touch/lcd_api.h"
#include "touch/touch_panel.h"

void dessiner_ligne(unsigned int x, unsigned int y, unsigned int l,unsigned int e, char orientation, unsigned short color)
{
	int i,j;
	if(orientation=='v')
	{
		for(j=y;j<=y+l;j++)
		{
			lcd_SetCursor(x,j);//on place le curseur � la bonne position
			rw_data_prepare();
			for(i=0;i<=e;i++)
			{
				write_data(color);//on trace un point et on passe � la position suivante
			}
		}
	}
	else//orientation='h'
	{
		for(j=y;j<=y+e;j++)
		{
			lcd_SetCursor(x,j);//on place le curseur � la bonne position
			rw_data_prepare();
			for(i=0;i<=l;i++)
			{
				write_data(color);//on trace un point et on passe � la position suivante
			}
		}
	}
}

void dessiner_rect(unsigned int x, unsigned int y, unsigned int lng, unsigned int lrg, unsigned int e, unsigned short plein, unsigned short e_color, unsigned short bg_color)
{
	//dessiner fond
	if(plein==1)
	{
		dessiner_ligne(x,y,lng,lrg,'h',bg_color);
	}

	//dessiner bordures
	dessiner_ligne(x,y,lng,e,'h',e_color);
	dessiner_ligne(x+lng-e,y,lrg,e,'v',e_color);
	dessiner_ligne(x,y+lrg-e,lng,e,'h',e_color);
	dessiner_ligne(x,y,lrg,e,'v',e_color);
}

// Trace UN seul pixel de couleur a la position (x,y).
static void dessiner_pixel(int x, int y, unsigned short color)
{
	lcd_SetCursor(x, y);
	rw_data_prepare();
	write_data(color);
}

// Trace le CONTOUR d'un cercle de centre (cx,cy) et de rayon r.
// Utilise l'algorithme du "midpoint circle" : que des additions d'entiers,
// pas de sin/cos ni de virgule flottante. Les 8 symetries du cercle
// permettent de tracer 8 pixels d'un coup a chaque etape.
static void dessiner_cercle(int cx, int cy, int r, unsigned short color)
{
	int x = r, y = 0;
	int err = 1 - r;            // variable de decision

	while(x >= y)
	{
		dessiner_pixel(cx + x, cy + y, color);
		dessiner_pixel(cx + y, cy + x, color);
		dessiner_pixel(cx - y, cy + x, color);
		dessiner_pixel(cx - x, cy + y, color);
		dessiner_pixel(cx - x, cy - y, color);
		dessiner_pixel(cx - y, cy - x, color);
		dessiner_pixel(cx + y, cy - x, color);
		dessiner_pixel(cx + x, cy - y, color);

		y++;
		if(err < 0)
			err += 2*y + 1;
		else
		{
			x--;
			err += 2*(y - x) + 1;
		}
	}
}

// Dessine un cadran d'horloge : un contour circulaire, 12 reperes d'heures
// (plus gros pour 12/3/6/9) et un moyeu central. (cx,cy) = centre en pixels.
// Dessin STATIQUE (fonctions bloquantes) : a appeler une seule fois.
void dessiner_horloge(unsigned int cx, unsigned int cy)
{
	// positions pre-calculees des 12 reperes (cercle de rayon 56)
	const int dx[12] = {   0,  28,  48,  56,  48,  28,   0, -28, -48, -56, -48, -28 };
	const int dy[12] = { -56, -48, -28,   0,  28,  48,  56,  48,  28,   0, -28, -48 };
	int i, x, y, taille;
	int cxi = (int)cx, cyi = (int)cy;

	// contour : 3 cercles concentriques pour un trait bien epais
	dessiner_cercle(cxi, cyi, 65, Black);
	dessiner_cercle(cxi, cyi, 64, Black);
	dessiner_cercle(cxi, cyi, 63, Black);

	// 12 reperes d'heures (carres pleins noirs, plus gros pour 12/3/6/9)
	for(i=0; i<12; i++)
	{
		taille = (i % 3 == 0) ? 8 : 5;       // repere plus gros aux quarts d'heure
		x = cxi + dx[i] - taille/2;          // centre le repere sur le point
		y = cyi + dy[i] - taille/2;
		dessiner_rect(x, y, taille, taille, 1, 1, Black, Black);
	}

	// moyeu central
	dessiner_rect(cxi - 4, cyi - 4, 8, 8, 1, 1, Black, Black);
}

// Renvoie sin(s * 6 degres) * 100, pour s entier.
// Un pas = 6 degres, donc 60 pas = 360 degres = un tour complet (1 par seconde).
// On stocke seulement le 1er quart (0 a 90 deg) et on deduit le reste par symetrie
// -> pas de virgule flottante, que des entiers.
static int sin60(int s)
{
	static const int sintab[16] = {0,10,21,31,41,50,59,67,74,81,87,91,95,98,99,100};
	s = ((s % 60) + 60) % 60;            // ramene s dans 0..59
	if(s <= 15) return  sintab[s];       // quadrant 1 (0..90 deg)
	if(s <= 30) return  sintab[30 - s];  // quadrant 2 (90..180 deg)
	if(s <= 45) return -sintab[s - 30];  // quadrant 3 (180..270 deg)
	return            -sintab[60 - s];   // quadrant 4 (270..360 deg)
}

// Trace un segment de droite entre (x0,y0) et (x1,y1), dans N'IMPORTE quelle
// direction (y compris diagonale), grace a l'algorithme de ligne de Bresenham.
static void dessiner_segment(int x0, int y0, int x1, int y1, unsigned short color)
{
	int dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);   // valeur absolue de l'ecart en x
	int dy = (y1 > y0) ? (y1 - y0) : (y0 - y1);   // valeur absolue de l'ecart en y
	int sx = (x0 < x1) ? 1 : -1;                  // sens de progression en x
	int sy = (y0 < y1) ? 1 : -1;                  // sens de progression en y
	int err = dx - dy;
	int e2;

	while(1)
	{
		dessiner_pixel(x0, y0, color);
		if(x0 == x1 && y0 == y1) break;
		e2 = 2 * err;
		if(e2 > -dy) { err -= dy; x0 += sx; }
		if(e2 <  dx) { err += dx; y0 += sy; }
	}
}

// Trace une aiguille EPAISSE (2 px) entre le centre (cx,cy) et la pointe (cx+dx,cy+dy).
// On superpose 3 segments decales d'1 px pour epaissir dans toutes les directions.
static void tracer_aiguille(int cx, int cy, int dx, int dy, unsigned short color)
{
	dessiner_segment(cx,     cy,     cx + dx,     cy + dy,     color);
	dessiner_segment(cx + 1, cy,     cx + dx + 1, cy + dy,     color);   // decale en x
	dessiner_segment(cx,     cy + 1, cx + dx,     cy + dy + 1, color);   // decale en y
}

// Dessine l'aiguille des secondes de l'horloge centree en (cx,cy).
// 'seconde' (0..59) donne la position : un tour complet en 60 s.
// La fonction efface elle-meme l'aiguille precedente, donc on peut
// l'appeler a chaque seconde. NB : fonction bloquante -> a appeler dans le main.
void dessiner_aiguille(unsigned int cx, unsigned int cy, int seconde)
{
	static int s_prec = -1;              // memorise la position precedente (pour l'effacer)
	const int R = 45;                    // longueur de l'aiguille (plus courte que les reperes)
	int cxi = (int)cx, cyi = (int)cy;
	int s = ((seconde % 60) + 60) % 60;
	int dx, dy;

	// 1) effacer l'ancienne aiguille (on la redessine en blanc)
	if(s_prec >= 0)
	{
		dx =  R * sin60(s_prec)      / 100;
		dy = -R * sin60(s_prec + 15) / 100;   // +15 pas = +90 deg : cos = sin decale
		tracer_aiguille(cxi, cyi, dx, dy, White);
	}

	// 2) dessiner la nouvelle aiguille (en rouge)
	dx =  R * sin60(s)      / 100;
	dy = -R * sin60(s + 15) / 100;
	tracer_aiguille(cxi, cyi, dx, dy, Red);

	// 3) redessiner le moyeu (l'effacement a pu l'entamer)
	dessiner_rect(cxi - 4, cyi - 4, 8, 8, 1, 1, Black, Black);

	s_prec = s;                          // on retient pour pouvoir l'effacer au prochain appel
}

