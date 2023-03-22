#include <sys/socket.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <libgen.h>
#include <stdbool.h>

#define BUFFER_SIZE 1024

static char *const status_line[2] = {"200 OK", "500 Internal server error"};

int main(int argc, char *argv[])
{
    if (argc <= 3)
    {
        printf("usage: ./%s ip_address port_number filename\n", basename(argv[0]));
        return 1;
    }
    char *const ip = argv[1];
    uint16_t port = atoi(argv[2]);

    char *const file_name = argv[3];

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
        printf("errno is %d\n", errno);
    }
    else
    {
        char header_buf[BUFFER_SIZE];
        memset(header_buf, 0, BUFFER_SIZE);
        char *file_buf;
        struct stat file_stat;
        bool valid = true;
        int len = 0;

        if (stat(file_name, &file_stat) < 0)
        {
            valid = false;
        }
        else
        {
            if (S_ISDIR(file_stat.st_mode)) // 是目录
            {
                valid = false;
            }
            else if (file_stat.st_mode & S_IROTH) // Read by others. 有读权限
            {
                int fd = open(file_name, O_RDONLY); // 只读方式打开
                file_buf = calloc((file_stat.st_size + 1), sizeof(char));
                if (read(fd, file_buf, file_stat.st_size) < 0)
                {
                    valid = false;
                }
            }
            else
            {
                valid = false;
            }
        }

        if (valid)
        {
            // 相比于sprintf，snprintf有长度限制且会添加末尾的0，更安全

            ret = snprintf(header_buf, BUFFER_SIZE - 1, "%s %s\r\n", "HTTP/1.1", status_line[0]);
            len += ret;
            ret = snprintf(header_buf + len, BUFFER_SIZE - 1 - len, "Content-Length: %ld\r\n", file_stat.st_size);
            len += ret;
            ret = snprintf(header_buf + len, BUFFER_SIZE - 1 - len, "%s", "\r\n");
            struct iovec iv[2];
            iv[0].iov_base = header_buf;
            iv[0].iov_len = strlen(header_buf);
            iv[1].iov_base = file_buf;
            iv[1].iov_len = file_stat.st_size;
            ret = writev(connfd, iv, 2);
        }
        else
        {
            ret = snprintf(header_buf, BUFFER_SIZE - 1, "%s %s\r\n", "HTTP/1.1", status_line[1]);
            len += ret;
            ret = snprintf(header_buf + len, BUFFER_SIZE - 1 - len, "%s", "\r\n");
            send(connfd, header_buf, strlen(header_buf), 0);
        }
        close(connfd);
        free(file_buf);
    }
    close(sock);
    return 0;
}