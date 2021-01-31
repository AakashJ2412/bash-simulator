#include "../head/sighandler.h"
#include "../head/headers.h"
#include "../head/process.h"
#include "../head/prompt.h"

void sigc_handler(int signum)
{
    if(fgobj.id != -1)
        exerrflag = 1;
}

void sigz_handler(int signum)
{
    if (fgobj.id != -1)
    {
        exerrflag = 1;
        kill(fgobj.id, SIGTSTP);
        insert(fgobj.id, fgobj.name);
        printf("\n[%d]+\tStopped\t\t%s\n", llcnt, fgobj.name);
    }
}

void sig_handler(int signum)
{
    pid_t pid;
    int stat, flag = 0;
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    {
        if (flag == 0)
            flag = 1;
        find(pid);
        if (exflag == 0)
        {
            if (WIFEXITED(stat) == 0)
                printf("with pid %d terminated abnormally\n", pid);
            else
                printf("with pid %d terminated normally\n", pid);
            pid = waitpid(-1, &stat, WNOHANG);
            if (flag == 1)
                prompt();
        }
        return;
    }
}
