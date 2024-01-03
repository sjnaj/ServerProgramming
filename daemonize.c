#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

/** 让进程以守护进程的方式运行
 * 等价于函数daemon;
*/
bool daemonize(){
    __pid_t pid=fork();

    if(pid<0){//fork失败
        return false;
    }

    else if(pid>0){//父进程退出
        exit(0);
    }
    umask(0);//设置文件掩码，新进程创建文件的权限将是mode&0777(九个0反码)最高权限

    __pid_t sid=setsid();
    if(sid<0){
        return false;
    }
    if((chdir("/"))<0){
        return false;
    }

    //关闭三个描述符0，1，2
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);


    //消耗掉0，1，2描述符，防止被重新使用
    open("/dev/null",O_RDONLY);
    open("/dev/null",O_RDWR);
    open("/dev/null",O_RDWR);
    return  true;

}
