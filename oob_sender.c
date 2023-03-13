#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
//发送带外(out of bound)（紧急)数据

int main(int argc,char*argv[]){
    if(argc<2){
        printf("usage: ./%s ip_address port_number\n",basename(argv[0]));
    }
    char* const ip=argv[1];
    int port=atoi(argv[2]);
    struct sockaddr_in server_address;
    memset(&server_address,0,sizeof(server_address));
    server_address.sin_family=AF_INET;
    inet_pton(AF_INET,ip,&server_address.sin_addr);
    server_address.sin_port=htons(port);

    int sockfd=socket(PF_INET,SOCK_STREAM,0);
    assert(sockfd>=0);

    if(connect(sockfd,(struct sockaddr*)&server_address,sizeof(server_address))<0){
        printf("connect failed\n");
    }
    else{
        char* const oob_data="adc";
        char* const normal_data="123";
        send(sockfd,normal_data,strlen(normal_data),0);
        send(sockfd,oob_data,strlen(oob_data),MSG_OOB);
        send(sockfd,normal_data,strlen(normal_data),0);
    }
    close(sockfd);
    return 0;
}

