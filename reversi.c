#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>

#include "header.h"

/* ------ REVERSI ------ */

void play (int conn_fd) {
	do {
    // stuff
	} while (1) ;
}

/* -------- MAIN -------- */

int main (int argc, char * argv[]) {
  // determine client or server
  if (argc == 2 && !strcmp(argv[1], "-server")) {
    int conn_fd = listen_at_port(atoi(argv[1])) ;
  }
  else if (argc == 3 && !strcpy(argv[1], "-client")) {
  	int conn_fd = connect_ipaddr_port(argv[1], atoi(argv[2])) ;
  }
  else {
    fprintf(stderr, "Error: Invalid argument.\n");
    return 1;
  }
	
	play(conn_fd) ;
	shutdown(conn_fd, SHUT_RDWR) ;
  
  return 0;
}
