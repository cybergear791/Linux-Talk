#include<sys/types.h>	
#include<sys/socket.h>	/* basic socket definitions */
#include<netinet/in.h>	/* sockaddr_in{} and other Internet defns */
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<curses.h>
#include<iostream>
#include<thread>
#define PORT_NUMBER 8022
#define MAXLINE 200
// The client, as an argument, takes the IP of a server and attempts
// to connect to it.  

// To run this program, a server program must already have been
// launched.  To compile, do:

// gcc client.c -o client.x

// To run, do:

// ./client.x IP-of-the server.

// If the server is running on the local machine, you can do:

// ./client.x 127.0.0.1
void startup( void );
int readn(int sd,char *ptr,int size);
void terminate( void );
void screen (int connfd, int sockfd);
void sendThread(int sockfd);
void readThread(int connfd);
int main(int argc, char **argv)
{
    int connfd, sockfd, n;
	struct sockaddr_in	servaddr;

	if (argc != 2) {
	    fprintf( stderr, "Usage: %s <IPaddress>\n", argv[0] );
	    exit( 1 );
	}

	// Create a socket end-point for communication.  We use the
	// IPv4 format and a stream socket. 
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	    fprintf( stderr, "Socket error.  %s\n", strerror( errno ) );
	    exit( 2 );
	}
	if ( (connfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	    fprintf( stderr, "Socket error.  %s\n", strerror( errno ) );
	    exit( 2 );
	}
	// build a profile for the server to whom we are going to
	// communicate.  
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(PORT_NUMBER);
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
	    fprintf( stderr, "inet_pton error for %s\n", argv[1] );
	    exit( 3 );
	}

	// Attempt to connect to the server.
	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
	    fprintf( stderr, "connect error: %s\n", strerror( errno ) );
	    exit( 4 );
	}
	if (connect(connfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
	    fprintf( stderr, "connect error: %s\n", strerror( errno ) );
	    exit( 4 );
	}
    // Read User Key Strokes.
    screen(sockfd, connfd);
    
	if (n < 0) {
	    fprintf( stderr, "read error: %s", strerror( errno ) );
	    exit( 6 );
	}
	close( sockfd );
	close( connfd );
	exit(0);
}

void screen(int sockfd, int connfd){
        startup();
        move(0, 0);  // move the curser to the top- and left-most block.
        addstr("0123456789012345678901234567890123456789");
        addstr("0123456789012345678901234567890123456789");
        move(1, 0);  // move the curser to the second row, first column.
        addch('*');  // put a character at (1, 0)
        move(1, 20);
        addstr( "This string starts on column 21.");
        mvaddch(1, 79, '*'); // same as: move(1, 79); addch('*');
        mvaddch(22, 0, '*');
        move(12, 30);
        addstr( "--------------------------------------------------------------");
        mvaddch(22, 79, '*');
        refresh();  // this function call forces the screen to be updated.
                    // the following function moves the curser to (4, 5) and then
                    // writes a string.  It is equivalent to the following two stmts:
                    // move(4, 5);
                    // addstr("Type in a non-blank character, after it is echoed ");
                    //mvaddstr(4, 5, "Type in a couple of non-blank characters ");
                    //addstr("and then wait and watch!");
        refresh();
        
			std::thread SendThread(sendThread, sockfd);
			std::thread ReadThread(readThread, connfd);
			SendThread.join();
			ReadThread.join();
		
        terminate();
}
void sendThread(int connfd){
    for(int j = 0; j < 25; j++){
            for(int i = 12; i < 80; i++) {
                char data;
                readn(connfd, (char*)&data, sizeof(data));
                mvaddch(3+j, i, data );
                refresh();
            }
    }
}
void readThread(int sockfd){
    for(int j = 0; j < 25; j++){
            for(int i = 12; i < 80; i++) {
                char temp = getchar();
                mvaddch(13+j, i, temp );
                refresh();
                if( int w = send(sockfd, (char *)&temp, sizeof(temp), 0) < 0){
                    fprintf( stderr, "send() error");
                    exit( 5 );
                }
            }
    }
}
int readn(int sd,char *ptr,int size)
{
    int no_left,no_read;
    no_left = size;
    while (no_left > 0) {
        no_read = read(sd,ptr,no_left);
        if(no_read <0)  return(no_read);
        if (no_read == 0) break;
        no_left -= no_read;
        ptr += no_read;
    }
    return(size - no_left);
}

void startup( void )
{
    initscr();        /* activate curses                                            */
    curs_set(0);   /* prevent the cursor to be displayed                         */
    clear();        /* clear the screen that curses provides                      */
    noecho();        /* prevent the input chars to be echoed to the screen         */
    cbreak();        /* change the stty so that characters are delivered to the
                      program as they are typed--no need to hit the return key!  */
}

void terminate( void )
{
    mvcur( 0, COLS -1, LINES-1, 0 );
    clear();
    refresh();
    endwin();
}