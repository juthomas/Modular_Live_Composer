#ifndef NCURSES_UTILS_H
#define NCURSES_UTILS_H
#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAP_COLOR(x) ((x)*1000 / 255)
#define CUSTOM_INIT_COLOR(index, color) init_color(index, MAP_COLOR((color & 0xFF0000) >> 16), MAP_COLOR((color & 0x00FF00) >> 8), MAP_COLOR(color & 0x0000FF))

	
typedef struct s_ncurses
{
	WINDOW *bottom;
	WINDOW *top;
} t_ncurses;

enum E_CUSTOM_COLORS
{
	CUSTOM_COLOR_1 = 100,
	CUSTOM_COLOR_2,
	CUSTOM_COLOR_3,
};

enum E_CUSTOM_PAIRS
{
	CUSTOM_PAIR_1 = 1
};

t_ncurses curses_env;

void init_curses(t_ncurses *nstruct)
{
	// WINDOW *haut, *bas;
	initscr();
	start_color(); /* Start color 			*/
	CUSTOM_INIT_COLOR(CUSTOM_COLOR_1, 0x00FF00);
	CUSTOM_INIT_COLOR(CUSTOM_COLOR_2, 0x0000FF);
	init_pair(CUSTOM_PAIR_1, CUSTOM_COLOR_1, CUSTOM_COLOR_2);
	nstruct->top = subwin(stdscr, LINES / 2, COLS, 0, 0);			 // Créé une fenêtre de 'LINES / 2' lignes et de COLS colonnes en 0, 0
	nstruct->bottom = subwin(stdscr, LINES / 2, COLS, LINES / 2, 0); // Créé la même fenêtre que ci-dessus sauf que les coordonnées changent
	wbkgd(nstruct->bottom, COLOR_PAIR(CUSTOM_PAIR_1));
	wrefresh(nstruct->top);
	wrefresh(nstruct->bottom);
}

void exit_curses2(t_ncurses *nstruct)
{
	endwin();

	free(nstruct->bottom);
	free(nstruct->top);
}

void rectangle(WINDOW *win, int y1, int x1, int y2, int x2)
{
    mvwhline(win,y1, x1, 0, x2-x1);
    mvwhline(win, y2, x1, 0, x2-x1);
    mvwvline(win, y1, x1, 0, y2-y1);
    mvwvline(win, y1, x2, 0, y2-y1);
    mvwaddch(win,y1, x1, ACS_ULCORNER);
    mvwaddch(win,y2, x1, ACS_LLCORNER);
    mvwaddch(win,y1, x2, ACS_URCORNER);
    mvwaddch(win,y2, x2, ACS_LRCORNER);
}

void draw_sensors_infos(t_ncurses *nstruct, int index, float v_val, int val)
{
	int col_width = COLS / 12;
	int col_height = 8;

	// mvaddstr((index * 8) / (COLS), (index * 8) % COLS, "Bonjour");
// box(bas, ACS_VLINE, ACS_HLINE);
int largeur_max = COLS / col_width * col_width;
int case_x = (index * col_width) / (largeur_max) * col_height;
int case_y =  (index * col_width) % largeur_max;



rectangle(nstruct->top,case_x, case_y,
case_x+ col_height, case_y + col_width);

wmove(nstruct->top,case_x + 1, case_y + 1);


char c_val[16];
snprintf (c_val, 15, "%d", val);

waddstr(nstruct->top, c_val );



	wrefresh(nstruct->top);

}

#endif