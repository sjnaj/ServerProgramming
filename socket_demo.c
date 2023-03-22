
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <libgen.h>
#include <errno.h>
static bool stop = false;

// 触发时结束主循环
static void handle_term(int sig)
{
    stop = true;
}
int main(int argc, char *argv[])
{
    signal(SIGTERM, handle_term);
    if (argc <= 3)
    {
        printf("usage: ./%s is_address port_number backlog\n", basename(argv[0]));
        return 1;
    }
    char *const ip = argv[1];

    unsigned int port = atoi(argv[2]);
    unsigned int backlog = atoi(argv[3]);

    // inet ,tcp
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);
    struct sockaddr_in address;

    // 填充为0，网络编程惯用,相对于memset减少犯错（参数写反）的可能，但标准已弃用
    // for cases where you know you’re going to zero out a block of memory, bzero() will be a bit more direct (avoiding the checking and what not). No profiling necessary, since it’s the same underlying algorithm.
    // bzero(&address,sizeof(address));

    // memset may be optimized away (under the as-if rules) if the object modified by this function is not accessed again for the rest of its lifetime (e.g., gcc bug 8537). For that reason, this function cannot be used to scrub memory (e.g., to fill an array that stored a password with zeroes).

    // This optimization is prohibited for memset_explicit and memset_s: they are guaranteed to perform the memory write.

    // Third-party solutions for that include FreeBSD explicit_bzero or Microsoft SecureZeroMemory.
    // 可能被优化，(这里address随即被使用，不会被优化)memset_s最适合填充内存,而且有boundary check,但是gcc没有实现，https://stackoverflow.com/questions/50724726/why-didnt-gcc-or-glibc-implement-s-functions
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    // 调整端口字节序
    address.sin_port = htons(port);
    int ret = bind(sock, (struct sockaddr *)&address, sizeof(address));
    assert(ret != -1);

    // 实际可容纳backlog+1个连接
    ret = listen(sock, backlog);
    assert(ret != -1);

    printf("Successful initialization\n");

    sleep(20);
    struct sockaddr_in client;
    socklen_t client_addrlength;
    int connfd = accept(sock, (struct sockaddr *)&client, &client_addrlength);
    if (connfd < 0)
    {
        printf("errno is :%d\n", errno);
    }
    else
    {
        char remote[INET_ADDRSTRLEN];
        printf("connected with ip: %s and port: %d\n", inet_ntop(AF_INET, &client.sin_addr, remote, INET_ADDRSTRLEN), ntohs(client.sin_port));
        close(connfd);
    }
    while (!stop)
    {
        sleep(1);
    }
    close(sock);
    printf("close\n");
    return 0;
}
