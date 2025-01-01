
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>


int main(int argc,char** argv) {
    char *ptr,**pptr;
    char str[60];

    struct hostent *hptr;
    while(--argc > 0){
        ptr = *++argv;
        if( ( hptr = gethostbyname(ptr) ) == NULL ){
            printf("gethostbyname(%s) failed\n",ptr);
            continue;
        }  

        printf("official name: %s\n",hptr->h_name);
        for(pptr = hptr->h_aliases; *pptr != NULL; pptr++)
            printf("alias: %s\n",*pptr);
        switch (hptr->h_addrtype)
        {
        case AF_INET:
            pptr = hptr->h_addr_list;
            while (*pptr != NULL)
            {
                printf("address: %s\n",inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)));
                pptr++;
            }
            break;
        
        default:
            printf("unknown address family\n");
            break;
        }
            
       
    }
    return 0;
}
