#include <fcntl.h>

/// @brief set file to nonblock
/// @param fd 
/// @return old_option
int setnonblocking(int fd){
    int old_option=fcntl(fd,F_GETFL);
    int new_option=old_option|O_NONBLOCK;
    fcntl(fd,F_SETFL,new_option);
    return old_option;
}