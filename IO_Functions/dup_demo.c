#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <libgen.h>

int main(int argc, char *argv[])
{
    if (argc <= 2)
    {
        printf("usage: ./%s ip_address port_number\n", basename(argv[0]));
        return 1;
    }
    char *const ip = argv[1];
    uint16_t port = atoi(argv[2]);

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET, ip, &address.sin_addr);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);

    int ret = bind(sock, (struct sockaddr *)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(sock, 5);
    assert(ret != -1);

    struct sockaddr_in client;
    socklen_t client_addrlength = sizeof(client);
    int connfd = accept(sock, (struct sockaddr *)&client, &client_addrlength);
    if (connfd < 0)
    {
        printf("errno is: %d\n", errno);
    }
    else
    {
        close(STDOUT_FILENO);
        dup(connfd);      // 复制connfd指向的文件，返回最小可用文件描述符，即刚才关闭的标准输出文件描述符1
        printf("abcd\n"); // 将发送到connfd指向的socket上
        close(connfd);
    }
    return 0;
}