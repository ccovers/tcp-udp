#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define SIZE 1024

int main(int argc, char **argv)
{
    if(argc < 3)
    {
        printf("./program  ip  port!\n");
        return 0;
    }
    int ret;

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        perror("socket");
        return 0;
    }

    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(atoi(argv[2]));
    local.sin_addr.s_addr = inet_addr(argv[1]);
    ret = bind(fd, (struct sockaddr*)&local, sizeof(local));
    if(ret < 0)
    {
        perror("bind");
        return 0;
    }

    char buf[SIZE];
    struct sockaddr_in peer;
    int len = sizeof(peer);
    int num;
    while(1)
    {
        num = recvfrom(fd, buf, sizeof(buf)-1, 0, (struct sockaddr*)&peer, &len);
        if(num < 0)
        {
            perror("recvfrom");
            break;
        }
        else if(num == 0)
        {
            continue;
        }
    
        int conLen = buf[0]*0xff + buf[1];
        if(conLen <= 0 
           || conLen > (num - 2))
        {
            printf("recv length error\n");
            continue;
        }

        buf[num] = '\0';
        char content[SIZE] = {0};
        strncpy(content, buf+2, conLen);

        printf("[%s:%d]%s\n",inet_ntoa(peer.sin_addr), htons(peer.sin_port),content);
    }
    close(fd);
}
