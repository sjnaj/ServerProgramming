#include <netinet/in.h>
#include <stdio.h>
#include <bits/socket.h>
#include <arpa/inet.h>
void demo(void)
{
    uint32_t i = 0x12345678;
    uint32_t nl = htonl(i); // 底层是bswap指令
    uint32_t nl1 = __builtin_bswap32(i);
    printf("%x\n", i);
    printf("%x\n", nl);
    printf("%x\n", nl1);
    printf("%x\n", ntohl(nl));
}
void demo1(void)
{
    // 仅展示ipv4

    // 通用
    /* Structure describing a generic socket address.  */
    struct sockaddr sa;     // size=14+2=16
    sa.sa_family = AF_INET; // inet4
    for (uint8_t i = 0; i < 6; i++)
    {
        sa.sa_data[i] = 0x11; // port:0x1111,addr:0x11111111
    }
    // 拓展，通用的空间太小
    /*Structure large enough to hold any socket address (with the historical
     exception of AF_UNIX)*/
    struct sockaddr_storage ss;
    ss.ss_family = AF_INET;
    ss.__ss_align = 0; // sizeof(padding+align)==128bytes,sizeof(ss)=4+128
    // 没有align(ulong)的话，padding可能和family(uchat) 在一个四字节里，不好对齐
    // 64位的默认对齐值为8
    for (uint8_t i = 0; i < 6; i++)
    {
        ss.__ss_padding[i] = 0x11;
    }
    // 专用，常用的
    struct sockaddr_in si;

    struct in_addr ia;
    ia.s_addr = 0x11111111;
    si.sin_addr = ia;
    si.sin_family = AF_INET;
    si.sin_port = 0x1111;
    si.sin_zero[0] = 0; // padding make size==sizeof(struct sockaddr)

    // 使用时拓展的和专用的都要转为通用类型的地址指针,通常还要传入长度大小，因为通用类型不足以容纳ipv6和unix类型的地址信息
}
void demo2(void)
{

    struct in_addr ia;
    ia.s_addr = inet_addr("192.168.52.128");
    printf("0x%x\n", ia.s_addr);

    /* The interface of this function is completely stupid, it requires a
   static buffer.  We relax this a bit in that we allow one buffer for
   each thread.  */
    // static __thread char buffer[18];

    char *addr = inet_ntoa(ia); // 使用一个静态变量存储，不可重入
    printf("%s\n", addr);
    char addr_p[INET_ADDRSTRLEN];
    addr = inet_ntop(AF_INET, &ia, addr_p, INET_ADDRSTRLEN);
    printf("%s", addr);

    // inet_ntop可重入，因为存储的空间需要用户分配并传入
}

int main()
{

    demo();
    demo1();
    demo2();
}
