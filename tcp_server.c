#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <curses.h>

#define SIZE 1024
void* threadFun(void *arg)
{
    char buf[SIZE] = {0};
    int fd = (int)arg;
    while(1)
    {
        memset(buf, 0, SIZE);
        ssize_t recvLen = read(fd, buf, sizeof(buf)-1);
        if(recvLen > 0)
        {
            if(recvLen > 2)
            {
                buf[recvLen] = '\0';
                int length = buf[0]*0xff + buf[1];
                if(length > (recvLen - 1))
                {
                    printf("length parse error\n");
                    continue;
                }
                char content[SIZE] = {0};
                strncpy(content, buf+2, length);
                printf("[recv]%s\n", content);
            }
            else
            {
                printf("pack data less than 2, parse error\n");
            }
            
        }
        else if(recvLen == 0)
        {
            printf("read error!\n");
            break;
        }
    }
}



int main(int argc, char **argv)
{
    if(argc < 3)
    {
        printf("./program  ip  port!\n");
        return 0;
    }
    int ret;

    int fd = socket(AF_INET, SOCK_STREAM, 0);
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

    if(listen(fd, 5) < 0)
    {
        perror("listen");
        return 0;
    }
   
    while(1)
    {
        struct sockaddr_in peer;
        int addrlen = sizeof(peer);
        int client = accept(fd, (struct sockaddr*)&peer, &addrlen);
        if(client != -1)
        {
            printf("[recv connect]%s:%d\n",inet_ntoa(peer.sin_addr),htons(peer.sin_port));

            pthread_t id;
            ret = pthread_create(&id, NULL, threadFun, (void*)client);
            if(ret != 0)
            {
                perror("pthread_creat\n");
                continue;
            }
            pthread_detach(id);
        }
    }

}
