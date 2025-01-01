#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <netinet/in.h>




int main(int argc, char *argv[]) {
    int sfd, s;
    char buf[1024];
    ssize_t len, nread;
    struct addrinfo hints, *result, *rp;
    
    if (argc < 3) {
        fprintf(stderr, "usage: %s <ip> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;
    
    s = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        return 2;
    }
    
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1) continue;
        
        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) break;
        close(sfd);
    }
    
    freeaddrinfo(result);
    if (rp == NULL) {
        fprintf(stderr, "Failed to connect\n");
        return 2;
    }
    
    for (size_t j = 3; j < argc; ++j) {
        len = strlen(argv[j]) + 1;
        if (len > BUFSIZ) {
            fprintf(stderr, "Argument too long\n");
            exit(EXIT_FAILURE);
        }
        
        if (send(sfd, argv[j], len, 0) != len) {
            perror("send");
            close(sfd);
            exit(EXIT_FAILURE);
        }
        
        nread = recv(sfd, buf, sizeof(buf) - 1, 0);
        if (nread == -1) {
            perror("recv");
            close(sfd);
            exit(EXIT_FAILURE);
        } else if (nread == 0) {
            printf("Connection closed by peer\n");
            break;
        } else {
            buf[nread] = '\0';
            printf("Received: %s\n", buf);
        }
        
        printf("Received %zd bytes\n", nread);
    }
    
    close(sfd);
    exit(EXIT_SUCCESS);
}