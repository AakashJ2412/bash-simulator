#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/wait.h>
#include <signal.h>
#ifndef __VAR_H
    char shellhome[201];
    int exflag, procchk,llcnt,rootpid, exerrflag;
    char prevdir[300];
    struct node
    {
        int id;
        char name[100];
        struct node *next;
    } * p;
    struct fgpid
    {
        int id;
        char name[100];
    }fgobj;
#endif