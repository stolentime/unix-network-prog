#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>

//客户端 只有3个步骤
//1.创建UDP socket
//2.发送数据
//3.接收数据

void dgm_client(FILE *fp, int sockfd, struct sockaddr *servaddr, socklen_t addrlen)
{
    char sendline[1024], recvline[1024];

    int n;
    socklen_t len;
    struct sockaddr* preply_addr;
    preply_addr = (struct sockaddr *)malloc(addrlen);
    connect(sockfd,servaddr,addrlen);

    while (fgets(sendline, sizeof(sendline), fp) != NULL) {
        // if(strlen(sendline)== 1 && sendline[0] == '\n')
        //     sendline[0] = '\0';
        // sendline[strlen(sendline) - 1] = '\0';
        //2.发送数据
        // if (sendto(sockfd, sendline, strlen(sendline), 0, servaddr, addrlen) < 0) {
        //     perror("sendto error");
        //     return;
        //}
        //2.1 不再使用sendto ，使用write
        write(sockfd,sendline,strlen(sendline));

        //3.接收数据
        // n = recvfrom(
        //     sockfd, recvline, sizeof(recvline), 0, preply_addr, &len);
        //3.1 不再使用recvfrom ， 使用read
        n = read(sockfd,recvline,1024);
         
        // if (len != addrlen || memcmp(servaddr, preply_addr, len)!= 0) {
        //     printf("reply from unknown source %s:%d\n",
        //           inet_ntoa(((struct sockaddr_in *)preply_addr)->sin_addr),
        //           ntohs(((struct sockaddr_in *)preply_addr)->sin_port));
        //           continue;
        // }
        recvline[n] = '\0';
        fputs(recvline, stdout); 
    }
}

int main()
{
    int sockfd;
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    //1.创建UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8000);
    //servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    dgm_client(stdin,sockfd, &servaddr, sizeof(servaddr));

    close(sockfd);

    return 0;
}