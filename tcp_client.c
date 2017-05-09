#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>

#define SIZE 1024


int main(int argc, char **argv)
{
    if(argc < 3)
    {
        printf("./program  server_ip  server_port\n");
        return 0;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        perror("socket");
        return 0;
    }

    struct sockaddr_in remote;
    remote.sin_family = AF_INET;
    remote.sin_port = htons(atoi(argv[2]));
    remote.sin_addr.s_addr = inet_addr(argv[1]);
    int ret = connect(sock, (struct sockaddr*)&remote, sizeof(remote));
    if(ret < 0)
    {
        perror("connect");
        return 0;
    }

    char buf[SIZE];
    while(1)
    {
        memset(buf, '\0', sizeof(buf));
        printf("please enter : \n");
        short len = read(0, buf+2, sizeof(buf) - 1 - 2);
        if(len > 0)
        {
            buf[0] = (len&0xff00) >> 8;
            buf[1] = len&0x00ff;
            buf[len+2] = '\0';
            ret = send(sock, buf, len+2, 0);
            if(ret == -1)
            {
                perror("send");
                break;
            }
        }
    }
    close(sock);
}
