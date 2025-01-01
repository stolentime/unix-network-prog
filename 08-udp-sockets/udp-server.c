#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <netdb.h>
//简单的udp-server 只有4个步骤
//1.创建socket
//2.绑定端口
//3.recvfrom 函数接收数据时，如果数据到达快，recvfrom会一次返回所有数据，如果数据量比较大，就会分为多次返回
//4.sendto 函数将数据发送给对方

void do_echo(int sockfd,struct sockaddr_in *clientaddr,socklen_t *clientaddrlen)
{
    char buf[1024];
    socklen_t len = *clientaddrlen;
    int n;

    while (1){
        memset(buf,0,sizeof(buf));
        //3 recvfrom 函数接收数据时，如果数据到达快，recvfrom会一次返回所有数据，如果数据量比较大，就会分为多次返回
        //一直阻塞，直到有数据为止
        n = recvfrom(sockfd,buf,sizeof(buf),0,(struct sockaddr*)clientaddr,&len);
        //如果recvfrom函数返回值小于0，则表示出错
        if (n > 0)
            //4. sendto 函数将数据发送给对方
            sendto(sockfd,buf,n,0,(struct sockaddr*)clientaddr,len);
        len = *clientaddrlen;
        printf("recvfrom:%d\n",strlen(buf));
    }
}

int main()
{
    int sockfd;
    int clientaddr_len;
    struct sockaddr_in serveraddr,clientaddr;
    //1.创建socket
    sockfd = socket(AF_INET,SOCK_DGRAM,0);

    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(8000);
    //这里不能使用 inet_pton(AF_INET,INADDR_ANY,&serveraddr.sin_addr);
    //因为inet_pton的输入是点分十进制字符串，而INADDR_ANY是宏定义，不是字符串    
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    //2.绑定端口
    bind(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    
    clientaddr_len = sizeof(clientaddr);
    memset(&clientaddr,0,clientaddr_len);
    do_echo(sockfd,&clientaddr,&clientaddr_len);

    return 0;
}