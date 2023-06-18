#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>

#include "header.h"


int main(int argc, char * argv[]) {
    int conn_fd;
    char whoAmI;
    
    if (argc == 3 && !strcmp(argv[1], "-server")) {
        // server is white
        whoAmI = 'w';
        printf("Waiting for opponent...\n");
        conn_fd = listen_at_port(atoi(argv[2]));
        printf("Connected\n");
    }
    else if (argc == 4 && !strcmp(argv[1], "-client")) {
        // client is black
        whoAmI = 'b';
        conn_fd = connect_ipaddr_port(argv[2], atoi(argv[3]));
    }
    else {
        // exit if neither
        fprintf(stderr, "Error: Invalid argument.\n");
        return 1;
    }
    
    play(conn_fd, whoAmI);
    
    shutdown(conn_fd, SHUT_RDWR);
    return 0;
}
