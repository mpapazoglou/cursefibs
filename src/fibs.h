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

#define PORT "4321"
#define HOST "fibs.com"

#define MAXROW 1024
#define MAXCOL 1024

typedef struct {
  char myname[MAXROW];
  char oname[MAXROW];
  int matchlen;
  int mypoints;
  int opoints;
  int board[26];
  int turn;
  int mydice[2];
  int odice[2];
  int cube;
  int mydouble;
  int odouble;
  int isdoubled;
  int color;
  int direction;
  int home;
  int bar;
  int myhome;
  int ohome;
  int mybar;
  int obar;
  int canmove;
  int redoubles;
} fibsBoard;

int check_board_string(char *);
void interpret_board_string(fibsBoard *, char *);
FILE *fibs_connect(int *);

