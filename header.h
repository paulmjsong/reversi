#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <netinet/tcp.h>
#include <arpa/inet.h>
//#include <linux/socket.h>


/* ------ SERVER ------ */

int listen_at_port (int portnum) 
{
	int sock_fd = socket(AF_INET /*IPv4*/, SOCK_STREAM /*TCP*/, 0 /*IP*/) ;
	if (sock_fd == 0)  { 
		perror("socket failed : "); 
		exit(EXIT_FAILURE); 
	}
	int opt = 2 ;
	if (setsockopt(sock_fd, SOL_TCP, TCP_NODELAY, &opt, sizeof(opt)) != 0) {
		fprintf(stderr, "fail at setsockopt\n") ;
		exit(EXIT_FAILURE) ;
	}

	struct sockaddr_in address ; 
	bzero(&address, sizeof(address)) ; 	
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY /* localhost */ ; 
	address.sin_port = htons(portnum); 

	if (bind(sock_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind failed: "); 
		exit(EXIT_FAILURE); 
	} 

	if (listen(sock_fd, 16 /* the size of waiting queue*/) < 0) { 
		perror("listen failed : "); 
		exit(EXIT_FAILURE); 
	} 

	int addrlen = sizeof(address); 
	int conn_fd = accept(sock_fd, (struct sockaddr *) &address, (socklen_t*)&addrlen) ;
	if (conn_fd < 0) {
		perror("accept failed: "); 
		exit(EXIT_FAILURE); 
	}
	return conn_fd ;
}

/* ------ CLIENT ------ */

int connect_ipaddr_port (const char * ip, int port)
{
	int sock_fd ;
	sock_fd = socket(AF_INET, SOCK_STREAM, 0) ;
	if (sock_fd <= 0) {
		perror("socket failed : ") ;
		exit(EXIT_FAILURE) ;
	}
	int opt = 2 ;
	if (setsockopt(sock_fd, SOL_TCP, TCP_NODELAY, &opt, sizeof(opt)) != 0) {
		fprintf(stderr, "fail at setsockopt\n") ;
		exit(EXIT_FAILURE) ;
	}

	struct sockaddr_in address ;
	bzero(&address, sizeof(address)) ;
	address.sin_family = AF_INET ; 
	address.sin_port = htons(port) ; 
	if (inet_pton(AF_INET, ip, &address.sin_addr) <= 0) {
		perror("inet_pton failed : ") ; 
		exit(EXIT_FAILURE) ;
	} 

	if (connect(sock_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
		perror("connect failed : ") ;
		exit(EXIT_FAILURE) ;
	}
	return sock_fd ;
}
