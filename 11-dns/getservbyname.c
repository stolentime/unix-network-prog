#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#define MAXLINE 4096

int main(int argc,char** argv)
{
    int sockfd,n;
    char recvline[MAXLINE+1];
    struct sockaddr_in servaddr;
    struct in_addr **pptr;
    struct in_addr *inetaddrp[2];
    struct in_addr inetaddr;
    struct hostent *hp;
    struct servent *sp;
    if(argc != 3)
    {
        printf("usage: daytimecli <hostname> <service>\n");
        exit(0);
    }
    if((hp = gethostbyname(argv[1])) == NULL)
    {
        if(inet_aton(argv[1],&inetaddr) == 0)
        {
            fprintf(stderr,"error: %s\n",hstrerror(h_errno));
            exit(-1);
        }
        inetaddrp[0] = &inetaddr;
        inetaddrp[1] = NULL;
        pptr = inetaddrp;
    }
    else{
        pptr = (struct in_addr**)hp->h_addr_list;
    }

    if( ( sp = getservbyname(argv[2],"tcp")) == NULL)
    {
        fprintf(stderr,"error: %s\n",strerror(errno));
        exit(-1);
    }

    for(;*pptr != NULL; pptr++)
    {
        if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        {
            perror("socket");
            exit(-1);
        }
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = sp->s_port;
        servaddr.sin_addr = **pptr;
        bzero(&servaddr.sin_zero,8);
        memcpy(&servaddr.sin_addr,*pptr,sizeof(struct in_addr));
        printf("trying %s\n",inet_ntoa(servaddr.sin_addr));

        if(connect(sockfd,(struct sockaddr*)&servaddr,sizeof(struct sockaddr)) == 0)
        {
            break;
        }
        printf("connect error\n");
        close(sockfd);
    }
    if(*pptr == NULL)
    {
        fprintf(stderr,"error: %s\n",strerror(errno));
        exit(-1);
    }
    while((n = read(sockfd,recvline,MAXLINE)) > 0)
    {
        recvline[n] = '\0';
        if(fputs(recvline,stdout) == EOF)
        {
            perror("fputs");
        }
    }
    exit(0);
}