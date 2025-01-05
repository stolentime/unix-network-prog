#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>


int tcp_server(const char* ip, int port, int backlog)
{
    int listenfd;
    struct sockaddr_in servaddr;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        return -1;
    }
    int opt = 1;
    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt");
        return -1;
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    if(ip == NULL)
    {
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        if(inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0)
        {
            perror("inet_pton");
            return -1;
        }
    }

    if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        perror("bind");
        return -1;
    }
    if(listen(listenfd, backlog) < 0){
        perror("listen");
        return -1;
    }

    return listenfd;
}