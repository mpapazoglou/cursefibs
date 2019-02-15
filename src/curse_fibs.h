/*
    curseFibs - curses-based interface for playing backgammon on 
    the First Internet Backgammon Server (FIBS)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <ncurses.h>
#include <fibs.h>

typedef struct {
  WINDOW *win;
  int x, y;
  int visible;
  char message[MAXROW][MAXCOL];
  int bold[MAXROW];
  int nchar;
  int nline;
  int scroll_line;
} fibsInterface;

void print_message(fibsInterface *win, char *line, int bold);
void print_message2(fibsInterface *win, char *line, int bold);
void draw_fibs_board(fibsInterface, fibsBoard);
void resize_handler(void);
int interpret_fibs_message(char *, fibsInterface *msgwin,
         fibsInterface *gamewin, fibsInterface *notifywin, int contflag);

void refresh_scroll_window(fibsInterface *win);

