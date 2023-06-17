#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include <pthread.h>

#include "header.h"
#include "move.h"

pthread_mutex_t lock;

typedef struct thread_data{
	int argc;
	char ** argv;
} thread_data;

void* serverThread(void* arg) {
    thread_data* data = (thread_data*) arg;
    int conn_fd = listen_at_port(atoi(data->argv[2]));
    serverDrawBoard(&lock, conn_fd);
    shutdown(conn_fd, SHUT_RDWR);
    return NULL;
}

void* clientThread(void* arg) {
    thread_data* data = (thread_data*) arg;
    int conn_fd = connect_ipaddr_port(data->argv[2], atoi(data->argv[3]));
    clientDrawBoard(&lock, conn_fd);
    shutdown(conn_fd, SHUT_RDWR);
    return NULL;
}

int main (int argc, char * argv[]) {

 	pthread_t server_tid, client_tid;
    	thread_data data = { argc, argv };

	pthread_mutex_init(&lock, NULL);

    	if (argc == 3 && !strcmp(argv[1], "-server")) {
        	pthread_create(&server_tid, NULL, serverThread, &data);
    	}
   	else if (argc == 4 && !strcmp(argv[1], "-client")) {
        	pthread_create(&client_tid, NULL, clientThread, &data);
    	}
    	else {
        	fprintf(stderr, "Error: Invalid argument.\n");
        	return 1;
    	}

    	pthread_join(server_tid, NULL);
   	pthread_join(client_tid, NULL);

	pthread_mutex_destroy(&lock);

	return 0;

	/*
	int conn_fd;
        if (argc == 3 && !strcmp(argv[1], "-server")) {
        	conn_fd = listen_at_port(atoi(argv[2])) ;
		serverDrawBoard(conn_fd) ;
        }
        else if (argc == 4 && !strcmp(argv[1], "-client")) {
                conn_fd = connect_ipaddr_port(argv[2], atoi(argv[3])) ;
		clientDrawBoard(conn_fd) ;
        }
        else {
                fprintf(stderr, "Error: Invalid argument.\n");
                return 1;
        }

        shutdown(conn_fd, SHUT_RDWR) ;
	return 0;
	*/

	
}
