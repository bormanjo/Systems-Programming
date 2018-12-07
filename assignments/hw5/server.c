#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const int MAXLINE = 1000;

int open_listenfd(char *port) 
{
    struct addrinfo hints, *listp, *p;
    int listenfd, optval=1;

    /* Get a list of potential server addresses */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;             /* Accept connections */
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG; /* ... on any IP address */
    hints.ai_flags |= AI_NUMERICSERV;            /* ... using port number */
    getaddrinfo(NULL, port, &hints, &listp);

    /* Walk the list for one that we can bind to */
    for (p = listp; p; p = p->ai_next) {
        /* Create a socket descriptor */
        if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) 
            continue;  /* Socket failed, try the next */

        /* Eliminates "Address already in use" error from bind */
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,    //line:netp:csapp:setsockopt
                   (const void *)&optval , sizeof(int));

        /* Bind the descriptor to the address */
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
            break; /* Success */
        close(listenfd); /* Bind failed, try the next */
    }

    /* Clean up */
    freeaddrinfo(listp);
    if (!p) /* No address worked */
        return -1;

    /* Make it a listening socket ready to accept connection requests */
    if (listen(listenfd, 1) < 0) {
        close(listenfd);
	return -1;
    }
    return listenfd;
}

float echo(int connfd, float count){
    size_t n;
    char buf[MAXLINE];
    
    while((n = read(connfd, buf, MAXLINE)) != 0) { 
        printf("\tserver adding %s", buf);
        count += atof(buf);

        printf("\tnew sum: %4.8f\n", count);
        write(connfd, buf, n);
        
        memset(buf, 0, MAXLINE);
    }

    return count;
}

int main(int argc, char **argv){
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    char client_hostname[MAXLINE], client_port[MAXLINE];

    // Define the total
    float total = 0.0;

    listenfd = open_listenfd(argv[1]);
    
    while(1) {
        clientlen = sizeof(struct sockaddr_storage); 
        
        /* Important! */
        connfd = accept(listenfd, (struct sockaddr* restrict) &clientaddr, &clientlen);
        getnameinfo((const struct sockaddr*) &clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
        printf("Connected to (%s, %s)\n", client_hostname, client_port);
        total = echo(connfd, total);
        close(connfd);
    }
    
    exit(0);
}