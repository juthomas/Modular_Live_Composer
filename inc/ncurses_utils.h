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
	WINDOW *bottom_right;
} t_ncurses;

enum E_CUSTOM_COLORS
{
	CUSTOM_COLOR_1 = 100,
	CUSTOM_COLOR_2,
	CUSTOM_COLOR_3,
};

enum E_CUSTOM_PAIRS
{
	CUSTOM_PAIR_1 = 1,
	CUSTOM_PAIR_2
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
	init_pair(CUSTOM_PAIR_2, CUSTOM_COLOR_2, CUSTOM_COLOR_1);
	nstruct->top = subwin(stdscr, LINES / 2, COLS, 0, 0);							  // Créé une fenêtre de 'LINES / 2' lignes et de COLS colonnes en 0, 0
	nstruct->bottom = subwin(stdscr, LINES / 2, COLS / 2, LINES / 2, 0);			  // Créé la même fenêtre que ci-dessus sauf que les coordonnées changent
	nstruct->bottom_right = subwin(stdscr, LINES / 2, COLS / 2, LINES / 2, COLS / 2); // Créé la même fenêtre que ci-dessus sauf que les coordonnées changent
	wbkgd(nstruct->bottom, COLOR_PAIR(CUSTOM_PAIR_1));
	wbkgd(nstruct->bottom_right, COLOR_PAIR(CUSTOM_PAIR_2));
	wrefresh(nstruct->top);
	wrefresh(nstruct->bottom);
	wrefresh(nstruct->bottom_right);
}

void exit_curses2(t_ncurses *nstruct)
{
	endwin();

	free(nstruct->bottom);
	free(nstruct->top);
}

void write_value(t_ncurses *nstruct, char *text)
{
	static int current_line = 0;
	current_line++;
	if (current_line >= LINES / 2)
		current_line = 0;
	// int col_width = COLS / 12;
	// int col_height = 1;

	// int largeur_max = COLS / col_width * col_width;
	// int case_y = (index * col_width) / (largeur_max)*col_height;
	// int case_x = (index * col_width) % largeur_max;

	// mvhline(case_y, case_x, ' ', col_width);
	mvhline(current_line, 1, ' ', COLS / 2);

	wmove(nstruct->bottom, current_line, 1);
	waddstr(nstruct->bottom, text);
	wrefresh(nstruct->bottom);

	// waddstr(nstruct->bottom, text);
}

void write_mode(t_ncurses *nstruct, char *mode, char *beg_note)
{
	mvwhline(nstruct->bottom_right, 1, 1, ' ', COLS / 2);
	wmove(nstruct->bottom_right, 1, 1);
	char c_val[64];
	snprintf(c_val, 64, "mode : %s, beg_node : %s", mode, beg_note);
	waddstr(nstruct->bottom_right, c_val);
	wrefresh(nstruct->bottom_right);
}

void write_speed(t_ncurses *nstruct, int32_t speed)
{
	mvwhline(nstruct->bottom_right, 2, 1, ' ', COLS / 2);
	wmove(nstruct->bottom_right, 2, 1);
	char c_val[64];
	snprintf(c_val, 64, "speed : %d %03d %03d", speed / 1000000, speed / 1000 % 1000, speed % 1000);
	waddstr(nstruct->bottom_right, c_val);
	wrefresh(nstruct->bottom_right);
}

void show_euclidean_circle(t_ncurses *nstruct, int8_t circle, t_euclidean *euclidean)
{
	mvwhline(nstruct->bottom_right, 5 + circle * 4, 1, ' ', COLS / 2);
	mvwhline(nstruct->bottom_right, 4 + circle * 4, 1, ' ', COLS / 2);

	wmove(nstruct->bottom_right, 4 + circle * 4, 1 + euclidean->current_step);
	waddstr(nstruct->bottom_right, "|");

	mvwhline(nstruct->bottom_right, 6 + circle * 4, 1, ' ', COLS / 2);
	wmove(nstruct->bottom_right, 6 + circle * 4, 1);
	char c_val[64];
	snprintf(c_val, 64, "mess chance : %02d/100",euclidean->mess_chance);
	waddstr(nstruct->bottom_right, c_val);



	for (uint8_t steps = 0; steps < euclidean->euclidean_steps_length; steps++)
	{

		if (euclidean->euclidean_steps[steps] == -1)
		{
			wmove(nstruct->bottom_right, 5 + circle * 4, 1 + steps);
			waddstr(nstruct->bottom_right, "O");
		}
		else
		{
			wmove(nstruct->bottom_right, 5 + circle * 4, 1 + steps);
			waddstr(nstruct->bottom_right, "#");
		}

		// if (steps % euclidean->step_gap == 0)
		// {
		// 	euclidean->euclidean_steps[steps] = get_new_chord_from_list(euclidean->chords_list,
		// 																euclidean->chords_list_length, steps, euclidean->euclidean_steps);
		// 	euclidean->euclidean_steps[steps] |= (rand() % euclidean->octaves_size) << 8; // add octave property
		// 	if (LOG_ALL)
		// 		printf("New step : %d\n", euclidean->euclidean_steps[steps]);
		// }
		// else
		// {
		// 	euclidean->euclidean_steps[steps] = -1;
		// }
	}

	wrefresh(nstruct->bottom_right);
}

void rectangle(WINDOW *win, int y1, int x1, int y2, int x2)
{
	mvwhline(win, y1, x1, 0, x2 - x1);
	mvwhline(win, y2, x1, 0, x2 - x1);
	mvwvline(win, y1, x1, 0, y2 - y1);
	mvwvline(win, y1, x2, 0, y2 - y1);
	mvwaddch(win, y1, x1, ACS_ULCORNER);
	mvwaddch(win, y2, x1, ACS_LLCORNER);
	mvwaddch(win, y1, x2, ACS_URCORNER);
	mvwaddch(win, y2, x2, ACS_LRCORNER);
}

void draw_sensors_infos(t_ncurses *nstruct, int index, float v_val, int32_t val, char *name, int32_t max_val, int input_nu)
{
	int col_width = COLS / 12;
	int col_height = 8;

	// mvaddstr((index * 8) / (COLS), (index * 8) % COLS, "Bonjour");
	// box(bas, ACS_VLINE, ACS_HLINE);
	int largeur_max = COLS / col_width * col_width;
	int case_y = (index * col_width) / (largeur_max)*col_height;
	int case_x = (index * col_width) % largeur_max;

	// attron(COLOR_PAIR(CUSTOM_PAIR_1));
	for (int i = 1; i < 7; i++)
	{
		mvhline(i + case_y, 1 + case_x, ' ', col_width);
	}
	// attroff(COLOR_PAIR(CUSTOM_PAIR_1));

	rectangle(nstruct->top, case_y, case_x,
			  case_y + col_height, case_x + col_width);

	wmove(nstruct->top, case_y + 1, case_x + 1);
	waddstr(nstruct->top, name);
	wmove(nstruct->top, case_y + 2, case_x + 1);
	char c_val[32];
	snprintf(c_val, 32, "input : %d", input_nu);
	waddstr(nstruct->top, c_val);
	wmove(nstruct->top, case_y + 3, case_x + 1);
	// char c_val[32];
	snprintf(c_val, 32, "%d", val);

	strcat(c_val, " / ");
	char c_val2[8];
	snprintf(c_val2, 7, "%d", max_val);
	strcat(c_val, c_val2);

	waddstr(nstruct->top, c_val);
	wmove(nstruct->top, case_y + 4, case_x + 1);
	snprintf(c_val, 7, "%.2f", v_val);
	strcat(c_val, "v");
	waddstr(nstruct->top, c_val);

	int fill_char = (float)val / (float)max_val * (col_width - 2);

	wmove(nstruct->top, case_y + 6, case_x + 1);
	waddstr(nstruct->top, "[");
	wmove(nstruct->top, case_y + 6, case_x + col_width - 1);
	waddstr(nstruct->top, "]");

	for (int i = 0; i < fill_char; i++)
	{
		wmove(nstruct->top, case_y + 6, case_x + 2 + i);
		waddstr(nstruct->top, "#");
	}

	wrefresh(nstruct->top);
}

#endif