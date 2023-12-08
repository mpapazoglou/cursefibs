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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "fibs.h"

int check_board_string(char *line){

  int count, n;

  count = 0;

  for (n=0; n<strlen(line); n++){
    if (line[n]==':'){
      count++;
    }
  }

  return count;

}

void interpret_board_string(fibsBoard *board, char *line){
  char *cfield;
  int n;

      cfield = strtok(line,":");

//      board->myname = strtok(NULL,":");
//      board->oname = strtok(NULL,":");
      cfield = strtok(NULL,":");
      sprintf(board->myname,"%s",cfield);

      cfield = strtok(NULL,":");
      sprintf(board->oname,"%s",cfield);

      cfield = strtok(NULL,":");
      board->matchlen = atoi(cfield);

      cfield = strtok(NULL,":");
      board->mypoints = atoi(cfield);
    
      cfield = strtok(NULL,":");
      board->opoints = atoi(cfield);
    
      for (n=0; n<26; n++){
        cfield = strtok(NULL,":");
        board->board[n] = atoi(cfield);
      }
        
      cfield = strtok(NULL,":");
      board->turn = atoi(cfield);
        
      for (n=0; n<2; n++){
        cfield = strtok(NULL,":");
        board->mydice[n] = atoi(cfield);
      }
        
      for (n=0; n<2; n++){
        cfield = strtok(NULL,":");
        board->odice[n] = atoi(cfield);
      }
        
      cfield = strtok(NULL,":");
      board->cube = atoi(cfield);
    
      cfield = strtok(NULL,":");
      board->mydouble = atoi(cfield);
    
      cfield = strtok(NULL,":");
      board->odouble = atoi(cfield);
    
      cfield = strtok(NULL,":");
      board->isdoubled = atoi(cfield);
    
      cfield = strtok(NULL,":");
      board->color = atoi(cfield);
    
      cfield = strtok(NULL,":");
      board->direction = atoi(cfield);
    
      cfield = strtok(NULL,":");
      board->home = atoi(cfield);
    
      cfield = strtok(NULL,":");
      board->bar = atoi(cfield);
    
      cfield = strtok(NULL,":");
      board->myhome = atoi(cfield);
    
      cfield = strtok(NULL,":");
      board->ohome = atoi(cfield);
    
      cfield = strtok(NULL,":");
      board->mybar = atoi(cfield);
    
      cfield = strtok(NULL,":");
      board->obar = atoi(cfield);
    
      cfield = strtok(NULL,":");
      board->canmove = atoi(cfield);
    
      cfield = strtok(NULL,":");
      cfield = strtok(NULL,":");

      cfield = strtok(NULL,":");
      board->redoubles = atoi(cfield);
}


FILE *fibs_connect(int *msock){
 
  int retval, status = 0;
  int flags, n;
  struct addrinfo hints, *res, *rp;
  FILE *fd;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = 0;
  hints.ai_protocol = 0;
  hints.ai_canonname = NULL;
  hints.ai_addr = NULL;
  hints.ai_next = NULL;

  retval = getaddrinfo(HOST, PORT, &hints, &res);

  if (retval<0){
     fd = NULL;
     return fd;
  }

  msock[0] = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

/*
  flags = fcntl(msock[0], F_GETFL, 0);
  flags |= O_NONBLOCK;
  fcntl(msock[0], F_SETFL, flags);
*/
  
  fd = fdopen(msock[0], "r+");
  setlinebuf(fd);

  n = 0;
  rp = res;

  while (n==0){
    retval = connect(msock[0], res->ai_addr, res->ai_addrlen);
    if (retval<0){
      if (res->ai_next==NULL){
        n = -1;
      } else {
        rp = res->ai_next;
      }
    } else {
      n = 1;
    }
  }

  if (n<=0){
    fd = NULL;
  }

  return fd;
}
