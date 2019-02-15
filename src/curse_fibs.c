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

/* connect USERNAME PASSWORD  at login: prompt */

#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>
#include <curse_fibs.h>

int window_resize = 0;
int DEBUGFLAG = 0;

void resize_handler(){
  window_resize = 1;
}

void draw_fibs_board(fibsInterface gamewin, fibsBoard board){

  int j, k, n, m;
  char mycolor, ocolor;
  int myturn, candouble;
  int dice[2];
  char name[200];
  int lmargin;
  
//  wclear(gamewin.win);
//  box(gamewin.win, 0, 0);

  lmargin = (gamewin.x-45)/2;

  for (n=1; n<gamewin.x-1; n++){
    for (m=1; m<=20; m++){
      mvwprintw(gamewin.win,m,n," ");
    }
  }


  if (board.direction==-1){
    mvwprintw(gamewin.win,1,1+lmargin, 
        " 13 14 15 16 17 18       19 20 21 22 23 24");
    mvwprintw(gamewin.win,15,1+lmargin,
        " 12 11 10  9  8  7        6  5  4  3  2  1");

  } else {
    mvwprintw(gamewin.win,1,1+lmargin,
         "  1  2  3  4  5  6        7  8  9 10 11 12");
    mvwprintw(gamewin.win,15,1+lmargin,
         " 24 23 22 21 20 19       18 17 16 15 14 13");
  }

  if (board.color<0) {
    mycolor = 'X';
    myturn = -1;
    ocolor = 'O';
  } else {
    mycolor = 'O';
    myturn = 1;
    ocolor = 'X';
  }

  wmove(gamewin.win,2,2+lmargin);
  whline(gamewin.win,ACS_HLINE,42);
  wmove(gamewin.win,3,1+lmargin);
  wvline(gamewin.win,ACS_VLINE,11);
  wmove(gamewin.win,2,1+lmargin);
  whline(gamewin.win,ACS_ULCORNER,1);
  wmove(gamewin.win,14,1+lmargin);
  whline(gamewin.win,ACS_LLCORNER,1);
  wmove(gamewin.win,14,2+lmargin);
  whline(gamewin.win,ACS_HLINE,42);
  wmove(gamewin.win,3,44+lmargin);
  wvline(gamewin.win,ACS_VLINE,11);
  wmove(gamewin.win,14,44+lmargin);
  whline(gamewin.win,ACS_LRCORNER,1);
  wmove(gamewin.win,2,44+lmargin);
  whline(gamewin.win,ACS_URCORNER,1);
  wmove(gamewin.win,3,20+lmargin);
  wvline(gamewin.win,ACS_VLINE,11);
  wmove(gamewin.win,3,24+lmargin);
  wvline(gamewin.win,ACS_VLINE,11);
  wmove(gamewin.win,2,20+lmargin);
  wvline(gamewin.win,ACS_TTEE,1);
  wmove(gamewin.win,2,24+lmargin);
  wvline(gamewin.win,ACS_TTEE,1);
  wmove(gamewin.win,14,20+lmargin);
  wvline(gamewin.win,ACS_BTEE,1);
  wmove(gamewin.win,14,24+lmargin);
  wvline(gamewin.win,ACS_BTEE,1);

  mvwprintw(gamewin.win,8,21+lmargin,"BAR");

/*
#ifdef _DEBUG
  mvwprintw(gamewin.win,21,1,"isdoubled: %d  ",board.isdoubled);
  mvwprintw(gamewin.win,22,1,"turn: %d  ",board.turn);
  mvwprintw(gamewin.win,23,1,"mydouble: %d  ",board.mydouble);
  mvwprintw(gamewin.win,24,1,"odouble: %d  ",board.odouble);
#endif
*/

/*
  if (board.isdoubled>0){
    mvwprintw(gamewin.win,19,1,"%s has doubled",
           board.oname);
  }
*/

//  mvwprintw(gamewin.win,16,1,"%s - score: %d  %s - score: %d",
//         board.myname, board.mypoints, board.oname, board.opoints);
  mvwprintw(gamewin.win,16,1,"%d-point match",board.matchlen);
  mvwprintw(gamewin.win,17,1,"score: %s-%d  %s-%d",
          board.myname, board.mypoints, board.oname, board.opoints);

  mvwprintw(gamewin.win,18,1,"BAR: %c-%d %c-%d  OFF: %c-%d %c-%d  CUBE: %d",
             mycolor, board.mybar, ocolor, board.obar,
             mycolor, board.myhome, ocolor, board.ohome,board.cube);

  if (board.turn==myturn){
    sprintf(name,"%s",board.myname);
    candouble = board.mydouble;
    dice[0] = board.mydice[0];
    dice[1] = board.mydice[1];
    if (dice[0]>0){
      if (board.canmove>1){
        mvwprintw(gamewin.win,20,1,"Please move %d pieces",board.canmove);
      } else if (board.canmove==1){
        mvwprintw(gamewin.win,20,1,"Please move %d piece",board.canmove);
      } else {
        mvwprintw(gamewin.win,20,1,"%s can't move",board.myname);
      }
    }
  } else {
    candouble = board.odouble;
    sprintf(name,"%s",board.oname);
    dice[0] = board.odice[0];
    dice[1] = board.odice[1];
  }

  if (board.turn!=0){
    if (board.isdoubled>0){
      wattron(gamewin.win,WA_BOLD);
      mvwprintw(gamewin.win,19,1,"%s doubled",
             board.oname);
      wattroff(gamewin.win,WA_BOLD);
    } else if ((board.mydouble==0)&&(board.odouble==0)){
      wattron(gamewin.win,WA_BOLD);
      mvwprintw(gamewin.win,19,1,"%s doubled",
             board.myname);
      wattroff(gamewin.win,WA_BOLD);
    } else if (dice[0]==0){
       if (candouble>0){
         mvwprintw(gamewin.win,19,1,"Waiting for %s to roll or double",name);
       } else {
         mvwprintw(gamewin.win,19,1,"Waiting for %s to roll",name);
       }
    } else {
       mvwprintw(gamewin.win,19,1,"%s rolled %d and %d",name, dice[0], dice[1]);
    } 
  } else {
     mvwprintw(gamewin.win,19,1,"waiting to start new game");
  }


  for (n=0; n<24; n++){
    if (board.direction<0){
      if (n<12){
        k = 42 - 3*n + lmargin;
        if (n>=6){
           k = k - 6;
        }
      } else {
        k = 3*(n-12)+3 + lmargin;
        if (n>=18){
          k = k+6;
        }
      }
    } else {
      if (n<12){
        k = 3 + 3*n + lmargin;
        if (n>=6){
          k = k+6;
        }
      } else {
        k = 42 - 3*(n-12) + lmargin;
        if (n>=18){
          k = k-6;
        }
      }
    } 
    for (m=1; m<6; m++){
      if (board.direction<0){
        if (n<12){
          j = 14-m;
        } else {
          j = m+2;
        }
      } else {
        if (n<12){
          j = m+2;
        } else {
          j = 14-m;
        }
      }
      if ((board.board[n+1]>=m)){
        mvwprintw(gamewin.win,j,k,"O");
      } else if (-1*board.board[n+1]>=m){
        mvwprintw(gamewin.win,j,k,"X");
      } else {
        mvwprintw(gamewin.win,j,k," ");
      }
    }
    if (abs(board.board[n+1])>5){
      mvwprintw(gamewin.win,j,k,"%d",abs(board.board[n+1]));
    }
  }

  for (m=1; m<6; m++){
    if (board.mybar>=m){
      mvwprintw(gamewin.win,14-m,22+lmargin,"%c",mycolor);
    } else {
      mvwprintw(gamewin.win,14-m,22+lmargin," ");
    }
    if (board.obar>=m){
      mvwprintw(gamewin.win,m+2,22+lmargin,"%c",ocolor);
    } else {
      mvwprintw(gamewin.win,m+2,22+lmargin," ");
    }
  }
  if (board.mybar>5){
    mvwprintw(gamewin.win,11,22+lmargin,"%d",board.mybar);
  }
  if (board.obar>5){
    mvwprintw(gamewin.win,7,22+lmargin,"%d",board.obar);
  }

//  for (n=0; n<13; n++){
//    mvwprintw(gamewin.win,20,3*n+1,"%d",board.board[n]);
//    mvwprintw(gamewin.win,21,3*n+1,"%d",board.board[n+12]);
//  }
  wrefresh(gamewin.win);

}

int interpret_fibs_message(char *message, fibsInterface *msgwin,
            fibsInterface *gamewin, fibsInterface *notifywin, int contflag){
  char username[MAXCOL];
  char action[MAXCOL];
  int retval=0;
  int m, n, spaceix[MAXCOL];


  int nmin;
  static int bold = 0;
  static int notify = 0;

// this is a kludge, but if the first character is a space, then treat this as 
// a continuarion
  if (message[0]==32){
    contflag = 1;
  }

  if (contflag==0){
    bold = 0;
    notify = 0;
  }

  for (m=0; m<MAXCOL; m++){
    spaceix[m] = -1;
  }

  m = 0;

  nmin = 0;

  for (n=0; n<strlen(message); n++){
    if (message[n]==32){
      if (n>nmin){
        spaceix[m] = n;
        m++;
      } else {
        nmin++;
      }
    }
  }

// special case:
  if (strncmp(message,"+----------",11)==0){
    notify = 0;
    bold = 0;
  }

//char buf[32];
//sprintf(buf,"cont: %d",contflag);
//print_message(msgwin,buf,0);

// get username and action:
  if (contflag==0){
    if (m>=2){
      for (n=0; n<spaceix[0]; n++){
        username[n] = message[n];
      }
      for (n=(spaceix[0]+1); n<spaceix[1]; n++){
        action[n-spaceix[0]-1] = message[n];
      }
      username[spaceix[0]] = '\0';
      action[spaceix[1]-spaceix[0]-1] = '\0';
    } else {
      sprintf(username," ");
      sprintf(action," ");
    }

// notifications:
    if ((strncmp(action,"logs",4)==0)||
        (strncmp(action,"drop",4)==0)||
        (strncmp(action,"error",5)==0)||
        (strncmp(action,"with",4)==0)||
        (strncmp(action,"win",3)==0)||
        (strncmp(action,"and",3)==0)||
        (strncmp(action,"waves",4)==0)||
        (strncmp(action,"give",4)==0)||
        (strncmp(action,"accept",6)==0)||
        (strncmp(action,"make",4)==0)||
        (strncmp(action,"want",4)==0)||
        (strncmp(action,"information",11)==0)||
        (strncmp(action,"'join",5)==0)||
        (strncmp(username,"Warning:",4)==0)||
        (strncmp(username,"**",2)==0)||
        (strncmp(username,"Pipcounts:",10)==0)||
        (strncmp(username,"It's",4)==0)||
        (strncmp(username,"Starting",8)==0)||
        (strncmp(action,"roll",4)==0)||
        (strncmp(username,"Value",5)==0)||
        (strncmp(message,"Please move",11)==0)||
        (strncmp(message,"score in",8)==0)||
        (strncmp(message,"You are",7)==0)||
        (strncmp(message,"turn:",5)==0)||
        (strncmp(message,"unlimited",9)==0)||
        (strncmp(message,"The only possible",17)==0)||
        (strncmp(message,"points for",10)==0)||
        (strncmp(message,"Closed old",10)==0)||
        (strncmp(message,"Done.",5)==0)||
        (strncmp(message,"Bearing off:",12)==0)||
        (strncmp(action,"move",4)==0)){

          notify = 1;
    } else if ((strncmp(action,"double",6)==0)||
               (strncmp(action,"can't",5)==0)){
          notify = 2;
          retval = 2;
    } else {
          notify = 0;
    }

    if (strncmp(message,"** You tell",11)==0){
        notify = 0;
    }

    if ((strncmp(action,"want",4)==0)||
        (strncmp(action,"'join",5)==0)||
        (strncmp(username,"Warning:",4)==0)||
        (strncmp(action,"say",3)==0)||
        (strncmp(action,"tell",4)==0)||
        (strncmp(action,"double",6)==0)||
        (strncmp(action,"whisper",7)==0)||
        (strncmp(message,"** You tell",11)==0)||
        (strncmp(action,"kibitz",6)==0)){

          bold = 1;
    } else {
         bold = 0;
    }

    
  }

  if (notify==0){
      print_message(msgwin,message,bold);
  } else {
      print_message(notifywin,message,bold);
  }

  return retval;
}

int key_handler(fibsInterface *win, int key, FILE *fd ){

  int out = 0;
  static int showtext=1;
  static int ppos = 1;
  int y, x;
  int n; 
  static int current_line = 0;
  char message[MAXCOL];
  
  getyx(win->win, y, x);

  if ((key==KEY_BACKSPACE)||(key==7)){
    if (x>ppos){
      wmove(win->win,1,x-1);
      wprintw(win->win," ");
      wmove(win->win,1,x-1);
    }
    win->message[current_line][win->nchar-1] = '\0';
    if (win->nchar>0) {
      win->nchar = win->nchar-1;
    }
  } else if (key==KEY_PPAGE){
    out = -10;
  } else if (key==KEY_NPAGE){
    out = -11;
  } else if (key==10){
//    mvwprintw(cdat.gamewin,2,1,"return [%d]",key);
//    mvwprintw(win->win,2,1,"command: %s                  ",win->message[0]);
//    wrefresh(win->win);

    if ((strncmp(win->message[current_line],"quit",4)==0)||
        (strncmp(win->message[current_line],"bye",3)==0)){
      out = -1;
    } else if ((strncmp(win->message[current_line],"wave",4)==0)){
      out = -2;
    } else if ((strncmp(win->message[current_line],"notify",6)==0)){
      out = -3;
    } else if ((strncmp(win->message[current_line],"test",4)==0)){
      out = 2;
    } else if ((strncmp(win->message[current_line],"exit",4)==0)){
      out = -5;
    } else {
      out = 1;
//      win->message[current_line][strlen(win->message[current_line])] = 10;
//      fputs(win->message[current_line], win->fdout);
      sprintf(message,"%s\n",win->message[current_line]);
      sprintf(win->message[0],"%s",win->message[current_line]);
      if (win->nchar>0){
        fputs(message, fd);
//        fflush(fd);
      } else {
        out = -4;
      }
    }
    for (n=1; n<win->x-1; n++){
      mvwprintw(win->win,1,n," ");
    }

    if (ppos==1){
      for (n=(MAXROW-1); n>0; n--){
        sprintf(win->message[n],"%s",win->message[n-1]);
      }
    }

    current_line = 0;
    wmove(win->win,1,1);
    ppos = 1;
    win->nchar = 0;
    win->message[0][0] = '\0';
    showtext = 1;

  }  else if ((key==259)&&(ppos==1)) {
    if (current_line<(MAXROW-1)){
      current_line++;

      for (n=1; n<win->x-1; n++){
        mvwprintw(win->win,1,n," ");
      }
      mvwprintw(win->win,1,1,"%s",win->message[current_line]);
      win->nchar = strlen(win->message[current_line]);
    }
    
  }  else if ((key==258)&&(ppos==1)) {
    if (current_line>0){
      current_line--;

      for (n=1; n<win->x-1; n++){
        mvwprintw(win->win,1,n," ");
      }
      mvwprintw(win->win,1,1,"%s",win->message[current_line]);
      win->nchar = strlen(win->message[current_line]);
    
    }
  } else {


    if (((key>=32)&&(key<=125))){
      if (x<(win->x-1)){
        if (showtext==1){
          wprintw(win->win,"%c",key);
        } else {
          wprintw(win->win,"*");
        }
      }
      if (win->nchar<(MAXCOL-1)){
        win->message[current_line][win->nchar] = key;
        win->message[current_line][win->nchar+1] = '\0';
        win->nchar++;
      }
    }
  }

/*
#ifdef _DEBUG
    int y1, x1;
    getyx(win->win, y1, x1);
    mvwprintw(win->win,2,1,"key: %d   ",key);
    wmove(win->win,y1,x1);
#endif
*/

//  wrefresh(win->win);

  return out;

}

void print_message(fibsInterface *win, char *line, int bold){

  char tstring[MAXCOL];
  int n;

  if ((strlen(line)>0)&&(line[0]!=62)&&(line[0]!=13)){

    win->nline++;

    if ((win->scroll_line>0)&&(win->scroll_line<(win->y-1))){
      win->scroll_line = win->scroll_line + 1;
    }
      
// fix the line:
    for (n=0; n<MAXCOL; n++){
      tstring[n] = 32;
      if ((line[n]>31)&&(line[n]<127)){
        tstring[n] = line[n];
      } else {
        tstring[n] = 0;
      }
    }
  
// shift all lines in buffer down by one line:
    for (n=(MAXROW-1); n>0; n--){
      sprintf(win->message[n],"%s",win->message[n-1]);
       win->bold[n] = win->bold[n-1];
    }
// add current line to top:
    sprintf(win->message[0],"%s",tstring);
    win->bold[0] = bold;

    refresh_scroll_window(win);
  }

}

void refresh_scroll_window(fibsInterface *win){

  int n;
  char tstring[MAXCOL];

  for (n=0; n<win->y-2; n++){

    sprintf(tstring,"%-1023s",win->message[n + win->scroll_line]);
    tstring[win->x-2] = '\0';

    if (win->bold[n + win->scroll_line]==1){
      wattron(win->win,WA_BOLD);
    }
    mvwprintw(win->win,win->y-n-2,1,"%s",tstring);
    wattroff(win->win,WA_BOLD);


  }

  wrefresh(win->win);

}

void fibs_interface_create(fibsInterface *msgwin, fibsInterface *cmdwin, 
                           fibsInterface *gamewin, fibsInterface *notifywin, 
                           int updateflag){

  int xmax, ymax;

  initscr();
  raw();
  noecho();
  cbreak();
  keypad(stdscr,TRUE);
  refresh();

  if (updateflag==0){
    cmdwin->nchar = 0;
    notifywin->nchar = 0;
    msgwin->nchar = 0;
    cmdwin->nline = 0;
    notifywin->nline = 0;
    msgwin->nline = 0;
    notifywin->visible = 1;
    msgwin->scroll_line = 0;
    notifywin->scroll_line = 0;
  }

  getmaxyx(stdscr,ymax,xmax);

  gamewin->x = xmax/2;

  if (gamewin->x>=56)
    gamewin->x = 56;

  gamewin->y = ymax-3;

  cmdwin->x = xmax;
  cmdwin->y = 3;
  
  if (notifywin->visible==1){
    notifywin->x = xmax - gamewin->x;
    notifywin->y = ymax/4;
  } else {
    notifywin->x = 0;
    notifywin->y = 0;
  }

  msgwin->x = xmax - gamewin->x;
  msgwin->y = ymax - notifywin->y - cmdwin->y;

  gamewin->win = newwin(gamewin->y, gamewin->x, 0, 0);
  box(gamewin->win,0,0);

  cmdwin->win = newwin(cmdwin->y, cmdwin->x, gamewin->y, 0);
  box(cmdwin->win,0,0);

  if (notifywin->visible>0){
    if ((updateflag==0)||(updateflag==1)){
      wattron(msgwin->win,WA_BOLD);
    }
    notifywin->win = newwin(notifywin->y, notifywin->x, 0, gamewin->x);
    box(notifywin->win, 0, 0);
    wattroff(msgwin->win,WA_BOLD);
  }

  msgwin->win = newwin(msgwin->y, msgwin->x, notifywin->y, gamewin->x);

  if ((updateflag==0)||(updateflag==1)){
    wattron(msgwin->win,WA_BOLD);
  }
  box(msgwin->win, 0, 0);
  wattroff(msgwin->win,WA_BOLD);

  keypad(cmdwin->win, TRUE);
  wmove(cmdwin->win, 1, 1);

  refresh();
  wrefresh(gamewin->win);
//  wrefresh(gamewin->subwin);
  if (notifywin->visible>0){
    wrefresh(notifywin->win);
  }
  wrefresh(msgwin->win);
  wrefresh(cmdwin->win);
  
}

void curse_get_input(WINDOW *win, int show, const char *prompt, char *buf, int len){

  int m, n, ppos;
  int goflag = 1;
  char c;

  ppos = strlen(prompt);

  mvwprintw(win,1,1,"%s",prompt);
  wrefresh(win);
  n = 0;
  while (goflag==1){
    c = wgetch(win);
    if ((c=='\n')||(n>=(len-1))){
      buf[n] = '\0';
      goflag = 0;
      wmove(win,1,1);
      for (n=0; n<(ppos + strlen(buf)); n++){
        wprintw(win," ");
      }
      wmove(win,1,1);

    } else if (((c==7)||(c==KEY_BACKSPACE))&&(n>0)) { 
      buf[n-1] = '\0';
      n = n-1;
      for (m=1; m<=(ppos + n+1); m++){
        mvwprintw(win,1,m," ");
      }
      mvwprintw(win,1,1,"%s",prompt);
      if (show>0){
        wprintw(win,"%s",buf);
      } else {
        for (m=0; m<strlen(buf); m++){
          wprintw(win,"*");
        }
      }
    } else if (((c>=32)&&(c<=125))){
      if (show>0){
        wprintw(win,"%c",c);
      } else {
        wprintw(win,"*");
      }
     
      buf[n] = c;
      n++;
    } else {
    }
  };

}

int recv_to_eol(int sockfd, char *line){
  int n;
  int nread;
  char ch;
   
  int goflag=1;

  for (n=0; n<MAXCOL; n++){
    line[n] = 0;
  }
  n = 0;

  while (goflag>0){
    nread = recv(sockfd, &ch, 1, MSG_DONTWAIT);
    line[n] = ch;
    if ((n==(MAXCOL-1))||(ch=='\n')||(ch==EOF)||(nread<=0)){
      goflag = 0;
    } else {
      n++;
    }
  }
  
  line[n]='\0';
  return n;
  
}

int read_to_eol(FILE *fd, char *line){
  int n, ch;
   
  int goflag=1;

  for (n=0; n<MAXCOL; n++){
    line[n] = 0;
  }
  n = 0;

  while (goflag>0){
    ch = fgetc(fd);
    line[n] = ch;
    if ((n==(MAXCOL-1))||(ch=='\n')||(ch==EOF)){
      goflag = 0;
    } else {
      n++;
    }
  }
  
  line[n]='\0';
  return n;
  
}

int main ( int argc, char *argv[] ){


  fibsInterface msgwin, cmdwin, gamewin, notifywin, *win;

  fibsBoard board;

  int key, retval=0;
  int readflag, readreturn;
  int goflag = 2;
  int m, n, nmax;
  int msock;
  FILE *fd;

  char buf[MAXCOL];
  char buf2[MAXCOL];

  char username[MAXCOL], password[MAXCOL];

  char *str;

  int contflag;

  int flags;
  int len, M, nloop;
  struct addrinfo hints, *res, *rp;
  fd_set readfds;
  char c;
  struct timeval timeout;

  int current_window = 1;

  retval = 1;
  while ((retval=getopt(argc, argv, "v"))!=-1){
    switch (retval){
      case 'v':
        DEBUGFLAG=1;
        break;
    }
  }


  int iteration = 0;
  FILE *debug_file;

  if (DEBUGFLAG==1){
    debug_file = fopen("./cfibs_debug.out","w");
  }

// initialize name:
  sprintf(board.myname,"me");

// signal event handler for window resize:
  signal(SIGWINCH, (__sighandler_t)resize_handler);
  
// initialize curses interface:
  fibs_interface_create(&msgwin, &cmdwin, &gamewin, &notifywin, 0);

  goflag = 1;

  while (goflag>0){

// if goflag is 1, then we need to try to connect:
    if (goflag==1){

      fd = fibs_connect(&msock);
      if (fd==NULL){
        mvwprintw(msgwin.win,1,1,"connect error\n");
        wrefresh(msgwin.win);
        usleep(1000000);
        endwin();
        return -1;
      } else {
        goflag = 3;
        nmax = STDIN_FILENO;
        if (msock>nmax) nmax = msock;
      }

    }
  
    FD_ZERO(&readfds);
    FD_SET(msock, &readfds);
    FD_SET(STDIN_FILENO, &readfds);
  
    wrefresh(cmdwin.win);

    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

// select statement waits from input from the fibs socket and from standard input:
    retval = select(nmax+1, &readfds, NULL, NULL, NULL);

// has there been a window resize event?  if so, re-draw
    if (window_resize==1){
      endwin();
      fibs_interface_create(&msgwin, &cmdwin, &gamewin, &notifywin, 1);
      window_resize=0;
      refresh_scroll_window(&msgwin);
      refresh_scroll_window(&notifywin); 
  
    }
        
    draw_fibs_board(gamewin, board);
    wrefresh(cmdwin.win);
  
// input from fibs:
    if ((FD_ISSET(msock, &readfds))){
  
      if (DEBUGFLAG==1){
        fprintf(debug_file,"iteration: %5d,  select: %5d,  msock: %5d,  window_resize: %2d\n",
                iteration,retval,msock,window_resize);
        fflush(debug_file);
        iteration++;
      } 


      readflag = 1;
 
      while (readflag==1){
// read string from fibs socket:
//        readreturn = read_to_eol(fd, buf);
        readreturn = recv_to_eol(msock, buf);

        if (DEBUGFLAG==1){
          fprintf(debug_file, "read_to_eol: %d\n",readreturn);
          fflush(debug_file);
        }

        if (readreturn>0){

          if (DEBUGFLAG==1){
            fprintf(debug_file,"text: %s\n",buf);
            fflush(debug_file);
          }

// check for login request:
          if (strncmp(buf,"login:",6)==0){
  
            for (n=1; n<(cmdwin.x-1); n++){
              mvwprintw(cmdwin.win,1,n," ");
            }
    
            curse_get_input(cmdwin.win, 1, "username: ", username,64);
            curse_get_input(cmdwin.win, 0, "password: ", password,64);
  
            sprintf(buf2,"connect %s %s\n",username,password);
            fputs(buf2, fd);

// check for board redraw:
          } else if (strncmp(buf,"board:",6)==0){
  
            retval = check_board_string(buf);
  
            if (retval==52){
 
              interpret_board_string(&board, buf);
              draw_fibs_board(gamewin, board);
              wrefresh(cmdwin.win);
//              wrefresh(gamewin.win);
  
            }
        
// otherwise send the string buffer to the normal logic and send to "interpret_fibs_message:
          } else {

            contflag = 0;
            len = strlen(buf);
            nloop = strlen(buf)/(msgwin.x-2) + 1;

            for (n=0; n<nloop; n++){
              if (len<(msgwin.x-2)){
                 M = len;
              } else {
                 M = msgwin.x-2;
              }
              for (m=0; m<M; m++){
                buf2[m] = buf[n*(msgwin.x-2)+m];
              }
              buf2[M] = '\0';
              retval = interpret_fibs_message(buf2, &msgwin, &gamewin,
                   &notifywin, contflag);
              contflag = 1;
              len = len-M;
  
              if (retval==2){
                sprintf(buf2,"board\n");
                fputs(buf2, fd);
                fflush(fd);
              }
            }
  
          }
        } else {
          readflag = 0;
        } 
      }   // this end the while loop for reading from the fibs socket
    }  // this end the if statement checking for input from the fibc socket

// from stdin:
    if (FD_ISSET(STDIN_FILENO, &readfds)){
  
      key = wgetch(cmdwin.win);
  
      if (key==9){
         current_window = 3-current_window;

         if (current_window==1) {
            wattron(msgwin.win,WA_BOLD);
            box(msgwin.win,0,0);
            wattroff(msgwin.win,WA_BOLD);
            wrefresh(msgwin.win);
            wattroff(notifywin.win,WA_BOLD);
            box(notifywin.win,0,0);
            wrefresh(notifywin.win);
         } else {
            wattron(notifywin.win,WA_BOLD);
            box(notifywin.win,0,0);
            wattroff(notifywin.win,WA_BOLD);
            wrefresh(notifywin.win);

            wattroff(msgwin.win,WA_BOLD);
            box(msgwin.win,0,0);
            wrefresh(msgwin.win);
         }

           

      }
      retval = key_handler(&cmdwin, key, fd);
  
/*
#ifdef _DEBUG
  mvwprintw(gamewin.win, 26, 1, "key_handler: %3d",retval);
  wrefresh(gamewin.win);
#endif
*/

      if (retval==-1){
        fputs("exit\n",fd);
//        fflush(fd);
        goflag = 1;
      } else if (retval==-2){
        fputs("wave\n",fd);
//        fflush(fd);
        goflag--;
      } else if (retval==-3){
        notifywin.visible = 1-notifywin.visible;
        fibs_interface_create(&msgwin, &cmdwin, &gamewin, &notifywin, 1);
      } else if (retval==-4){
        sprintf(buf2,".     ");
        print_message(&msgwin, buf2,0);              
      } else if (retval==-5){
        fputs("exit\n",fd);
        goflag = 0;
      } else if (retval==-10){

        if (current_window==1){
           win = &msgwin;
        } else {
           win = &notifywin;
        }

        if (win->scroll_line < (win->nline  - win->y + 3)){
          win->scroll_line = win->scroll_line + win->y/2;
        }

        if (win->scroll_line>(MAXCOL-win->y))
            win->scroll_line = MAXCOL-win->y; 

        refresh_scroll_window(win);
      } else if (retval==-11){

        if (current_window==1){
           win = &msgwin;
        } else {
           win = &notifywin;
        }


        win->scroll_line = win->scroll_line - win->y/2;
        if (win->scroll_line<0) 
             win->scroll_line = 0;
        refresh_scroll_window(win);
  
      }  
    }

    fflush(stdin);

// check the fibs socket to see if it is still open:
    if (feof(fd)!=0){
       key = 0;
       mvwprintw(cmdwin.win,1,1,"connection to fibs has been closed... hit return to try to reconnect");
       while (key!=10){
         key = wgetch(cmdwin.win);
       }
       goflag = 1;
    }


  }
  endwin();

#ifdef _DEBUG
  fclose(debug_file);
#endif
  return retval;

}
