/*
 The  following programs demonstrate the use of getaddrinfo(), gai_strerror(), freeaddrinfo(), and getnameinfo(3).  The programs are an echo server
       and client for UDP datagrams.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#define BUFSIZE 256
#define PORT "3333"
#define HOST "127.0.0.1"

int main(int argc, char *argv[]) {
    int sfd, s;
    char buffer[256];
    ssize_t nread;
    socklen_t peer_addrlen;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    struct sockaddr_storage peer_addr;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if ((s = getaddrinfo(NULL, argv[1], &hints, &result)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    // Try each address until we successfully bind.
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1) continue;
        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0) break;
        close(sfd);
    }

    freeaddrinfo(result);
    if (rp == NULL) {
        fprintf(stderr, "Could not bind socket\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        peer_addrlen = sizeof(struct sockaddr_storage);
        nread = recvfrom(sfd, buffer, sizeof(buffer), 0,
                        (struct sockaddr *)&peer_addr, &peer_addrlen);
        if (nread == -1) {
            perror("recvfrom");
            continue;
        }

        s = getnameinfo((struct sockaddr *)&peer_addr, peer_addrlen,
                        buffer, sizeof(buffer), NULL, 0, NI_NUMERICHOST);
        if (s == 0) {
            printf("Received %zd bytes from %s\n", nread, buffer);
        } else {
            fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));
        }

        if (sendto(sfd, buffer, nread, 0,
                   (struct sockaddr *)&peer_addr, peer_addrlen) != nread) {
            perror("sendto");
        }
    }

    close(sfd); // Moved here to ensure the socket is closed properly.
    return EXIT_SUCCESS;
}