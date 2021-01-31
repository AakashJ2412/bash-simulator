#include "../head/headers.h"
#include "../head/cd.h"
#include "../head/sighandler.h"

void setenvar(char st[])
{
    int wcnt = 0;
    for (int i = 1; i < strlen(st); i++)
    {
        if (st[i] == '\t')
            st[i] = ' ';
        if (st[i] == ' ' && (st[i + 1] == ' ' || st[i + 1] == '\n' || st[i + 1] == '\t'))
            continue;
        if (st[i] == ' ')
            wcnt++;
    }
    if (wcnt == 1)
    {
        exerrflag = 1;
        printf("Error: Too few arguments.\n");
        return;
    }
    else if (wcnt > 3)
    {
        exerrflag = 1;
        printf("Error: Too many arguments.\n");
        return;
    }
    char *envtok = strtok(st, " ");
    envtok = strtok(NULL, " ");
    if (wcnt == 2)
    {
        if (setenv(envtok, "", 1) != 0)
        {
            exerrflag = 1;
            printf("Error: Variable could not be created");
        }
        return;
    }
    char envname[50];
    strcpy(envname, envtok);
    envtok = strtok(NULL, " ");
    if (setenv(envname, envtok, 1) != 0)
    {
        exerrflag = 1;
        printf("Error: Variable could not be created.\n");
    }
}

void unsetenvar(char st[])
{
    int wcnt = 0;
    for (int i = 1; i < strlen(st); i++)
    {
        if (st[i] == '\t')
            st[i] = ' ';
        if (st[i] == ' ' && (st[i + 1] == ' ' || st[i + 1] == '\n' || st[i + 1] == '\t'))
            continue;
        if (st[i] == ' ')
            wcnt++;
    }
    if (wcnt == 1)
    {
        exerrflag = 1;
        printf("Error: Too few arguments.\n");
        return;
    }
    else if (wcnt > 2)
    {
        exerrflag = 1;
        printf("Error: Too many arguments.\n");
        return;
    }
    char *envtok = strtok(st, " ");
    envtok = strtok(NULL, " ");
    if(unsetenv(envtok) != 0)
    {
        exerrflag = 1;
        printf("Error: Variable could not be deleted.\n");
    }
}