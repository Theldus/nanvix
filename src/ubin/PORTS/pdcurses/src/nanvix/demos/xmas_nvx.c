/* Nanvix PDCurses demo.
 * Released in public domain.
 */

#include <curses.h>
#include <unistd.h>

static void xmas(void)
{
	static int color1 = 1;
	static int color2 = 2;

	attron(COLOR_PAIR(color1) | A_BOLD);
	mvprintw(0,  30, "   __     __)               \n");
	mvprintw(1,  30, "  (, /|  /|                 \n");
	mvprintw(2,  30, "    / | / |   _  __  __     \n");
	mvprintw(3,  30, " ) /  |/  |__(/_/ (_/ (_(_/_\n");
	mvprintw(4,  30, "(_/   '                .-/  \n");
	mvprintw(5,  30, "                      (_/   \n");

	attron(COLOR_PAIR(color2) | A_BOLD);
	mvprintw(6,  30, " __   __)                   \n");
	mvprintw(7,  30, "(,  |/                      \n");
	mvprintw(8,  30, "    |  ___   _   _          \n");
	mvprintw(9,  30, " ) /|_ // (_(_(_/_)_        \n");
	mvprintw(10, 30, "(_/                         \n");

	color1 = 3 - color1;
	color2 = 3 - color2;
}

int main(void)
{
	initscr();			/* Start curses mode.     */
	start_color();      /* Enable colors.         */
	curs_set(0);        /* Invisible cursor.      */
	timeout(0);         /* Non-blocking read key. */

	/* Initialize color pairs. */
	init_pair(1, COLOR_GREEN,  COLOR_BLACK);
	init_pair(2, COLOR_RED,    COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);
	init_pair(4, COLOR_BLUE,   COLOR_BLACK);
	init_pair(5, COLOR_CYAN,   COLOR_BLACK);
	init_pair(6, COLOR_MAGENTA,COLOR_BLACK);
	init_pair(7, COLOR_WHITE,  COLOR_BLACK);

	attron(COLOR_PAIR(7) | A_NORMAL);
	mvprintw(23, 0,  "Press ESC to exit");
	mvprintw(23, 50, "by David ;-)");

	refresh();

	/* Attributes. */
	attron(COLOR_PAIR(1) | A_BOLD);

	int balls_color[] = {1, 2, 3, 4, 5, 6, 7};
	int balls[] = {0, 4, 6, 5, 2, 1, 3, 0, 6};

	while(1)
	{
		move(0, 0);
		attron(COLOR_PAIR(1) | A_BOLD);
		printw("           .\n");
		printw("         _.+,_\n");
		printw("          '+`\n");
		printw("          / \\\n");
		printw("         /`,o\\\n");
		printw("        /_* ~_\\\n");
		printw("        / o .'\\\n");
		printw("       /_,~' *_\\\n");
		printw("       /`. *  *\\\n");
		printw("      /   `~. o \\\n");
		printw("     /_ *    `~,_\\\n");
		printw("     /   o  *  ~'\\\n");
		printw("    / *    .~~'  o\\\n");
		printw("   /_,.~~'`    *  _\\\n");
		printw("   /`~..  o        \\\n");
		printw("  / *   `'~..   *   \\\n");
		printw(" /_     o    ``~~.,,_\\\n");
		printw(" /  *      *     ..~'\\\n");
		printw("/*    o   _..~~`'*   o\\\n");
		printw("`+.__.~'`'   *   ___.+'\n");
		printw("      \":+-----+:\"\n");
		printw("   hjw  \\_____/\n");

		/* Balls color. */
		mvaddch(4, 12, 'o' | COLOR_PAIR( balls_color[balls[0]] ) | A_BOLD);
		balls[0] = (balls[0] + 1) % 7;

		mvaddch(6, 10, 'o' | COLOR_PAIR( balls_color[balls[1]] ) | A_BOLD);
		balls[1] = (balls[1] + 1) % 7;

		mvaddch(9, 14, 'o' | COLOR_PAIR( balls_color[balls[2]] ) | A_BOLD);
		balls[2] = (balls[2] + 1) % 7;

		mvaddch(11, 9, 'o' | COLOR_PAIR( balls_color[balls[3]] ) | A_BOLD);
		balls[3] = (balls[3] + 1) % 7;

		mvaddch(12, 17, 'o' | COLOR_PAIR( balls_color[balls[4]] ) | A_BOLD);
		balls[4] = (balls[4] + 1) % 7;

		mvaddch(14, 10, 'o' | COLOR_PAIR( balls_color[balls[5]] ) | A_BOLD);
		balls[5] = (balls[5] + 1) % 7;

		mvaddch(16, 8, 'o' | COLOR_PAIR( balls_color[balls[6]] ) | A_BOLD);
		balls[6] = (balls[6] + 1) % 7;

		mvaddch(18, 6, 'o' | COLOR_PAIR( balls_color[balls[7]] ) | A_BOLD);
		balls[7] = (balls[7] + 1) % 7;

		mvaddch(18, 21, 'o' | COLOR_PAIR( balls_color[balls[8]] ) | A_BOLD);
		balls[8] = (balls[8] + 1) % 7;

		/* Merry Xmas. */
		xmas();

		/* Messages. */
		attron(COLOR_PAIR(7) | A_NORMAL);
		mvprintw(23, 0,  "Press ESC to exit");
		mvprintw(23, 50, "by David ;-)");

		refresh();    /* Print it on to the real screen */
		napms(1000);  /* Wait for 1 sec.                */

		/* Exit. */
		if (getch() == 27)
			break;
	}

	endwin();
	return 0;
}
