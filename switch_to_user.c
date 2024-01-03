#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

/*将以root启动的进程切换为以普通用户运行*/
static bool switch_to_user(__uid_t user_id, __gid_t gp_id)
{
    if ((user_id == 0) && (gp_id == 0))
    { // 目标是root用户
        return false;
    }
    __gid_t gid = getgid();
    __uid_t uid = getuid();
    if (!((gid == 0) && (uid != 0)) || ((gid == gp_id) && (uid == user_id)))
    { // 确保当前用户是root或者目标
        return false;
    }
    if (uid != 0)
    { // 不是root，就是目标，不用转换
        return true;
    }
    if ((setgid(gp_id) < 0) || (setuid(user_id) < 0))
    {
        return false;
    }
    return true;
}