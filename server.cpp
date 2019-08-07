#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/time.h>
#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>
#include<iostream>
#include<curses.h>
#include<thread>
#define PORT_NUMBER 8022
#define MAXLINE 200
#define LISTENQ 5

    // This server program creates a TCP socket and listen on port "PORT_NUMBER".

    // To compile, do:
    // g++ server.cpp -o server.x
    // To run, do:
    // ./server.x

    // A server must be launched before clients can talk to it!
int readn(int sd,char *ptr,int size);
void startup( void );
void terminate( void );
void screen (int connfd, int sockfd);
void sendThread(int connfd);
void readThread(int sockfd);

int main(int argc, char **argv)
{
    int    listenfd, connfd, sockfd, n;
    socklen_t   len;
    struct sockaddr_in  servaddr, cliaddr;
    char    buff[MAXLINE];
    time_t    ticks;
    
        // Create an end-point for IPv4 Internet Protocol
    if( ( listenfd = socket(AF_INET, SOCK_STREAM, 0) ) < 0 ) {
        fprintf( stderr, "socket failed.  %s\n", strerror( errno ) );
        exit( 1 );
        
    }
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;  // Communicate using the Internet domain (AF_INET)
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);   // Who should we accept connections from?
    servaddr.sin_port        = htons(PORT_NUMBER);  // Which port should the server listen on?
    
        // Bind the server end-point using the specifications stored in "serveraddr"
    if( bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) {
        fprintf( stderr, "Bind failed.  %s\n", strerror( errno ) );
        exit( 1 );
    }
    
        // Listen on the in-comming connections; pile up at most LISTENQ number of connections.
    if( listen(listenfd, LISTENQ) < 0 ) {
        fprintf( stderr, "Listen failed.  %s\n", strerror( errno ) );
        exit( 1 );
    }
    
    for ( ; ; ) {
        len = sizeof(cliaddr);
            // establish a connection with an incoming client.
        if( ( connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &len) ) < 0 ) {
            fprintf( stderr, "Accept failed.  %s\n", strerror( errno ) );
            exit( 1 );
        }
        if( ( sockfd = accept(listenfd, (struct sockaddr *) &cliaddr, &len) ) < 0 ) {
            fprintf( stderr, "Accept failed.  %s\n", strerror( errno ) );
            exit( 1 );
        }
        printf("connection from %s, port %d\n",
               inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)),
               ntohs(cliaddr.sin_port));
        
        screen(connfd, sockfd);
       
        // finished talking to this client.  Close the connection.
        close(connfd);
        close(sockfd);
        fprintf(stderr, "[connection closed]\n");
    }
}

void screen(int connfd, int sockfd){
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
