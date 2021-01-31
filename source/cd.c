#include "../head/headers.h"
#include "../head/cd.h"
#include "../head/sighandler.h"

void cd(char st[])
{
    int flag = 0;
    for (int i = 2; i < strlen(st); i++)
    {
        if (st[i] == '\n' || st[i] == '\t')
            st[i] = ' ';
        if ((st[i] == ' ' || st[i] == '\t') && flag == 0)
            continue;
        else if (flag == 0 && (st[i] != ' ' && st[i] != '\t'))
        {
            flag = 1;
        }
        else if (flag == 1 && (st[i] == ' ' || st[i] == '\t'))
        {
            flag = 2;
        }
        else if (flag == 2 && (st[i] != ' ' && st[i] != '\t' && st[i] != '\n'))
        {
            exerrflag = 1;
            write(1, "Error: cd has too many arguments\n", strlen("Error: cd has too many arguments\n"));
            return;
        }
    }
    char *tok = strtok(st, " ");
    tok = strtok(NULL, " ");
    char add[201], dirpwd[302];
    getcwd(dirpwd, 300);
    if (tok[0] == '~')
    {
        for (int i = 0; i < strlen(tok); i++)
        {
            tok[i] = tok[i + 1];
        }
        sprintf(add, "%s%s", shellhome, tok);
    }
    else if (tok[0] == '-')
    {
        if (strcmp(prevdir, "NULL_DIR") == 0)
        {
            exerrflag = 1;
            printf("Error: Previous Directory not set\n");
            return;
        }
        else
        {
            strcpy(add, prevdir);
        }
    }
    else
    {
        strcpy(add, tok);
    }
    if (chdir(add) != 0)
    {
        exerrflag = 1;
        perror("Error: ");
        return;
    }
    strcpy(prevdir,dirpwd);
}
