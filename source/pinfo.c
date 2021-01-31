#include "../head/headers.h"
#include "../head/pinfo.h"
#include "../head/sighandler.h"

void pinfo(char st[])
{
    if (st[5] != ' ' && st[5] != '\t' && st[5] != '\n')
    {
        exerrflag = 1;
        printf("Error: Command not Found\n");
        return;
    }
    int f = 0, fg = 0, val = 0;
    for (int i = 5; i < strlen(st); i++)
    {
        if (st[i] == ' ' || st[i] == '\t' || st[i] == '\n')
            continue;
        if (st[i] >= '0' && st[i] <= '9')
        {
            fg = 1;
            val = val * 10;
            val += st[i] - '0';
        }
        else
        {
            exerrflag = 1;
            printf("Invalid Input for PID\n");
            return;
        }
    }
    int pid;
    if (fg == 1)
        pid = val;
    else
        pid = getpid();
    char file[20];
    sprintf(file, "/proc/%d/stat", pid);
    FILE *fp = fopen(file, "r");
    if (fp == NULL)
    {
        exerrflag = 1;
        char msg[200];
        sprintf(msg, "Error: Process with PID %d doesnt exist\n", pid);
        write(1, msg, strlen(msg));
        return;
    }
    char filestr[1001];
    if (fgets(filestr, 1000, fp) != NULL)
    {
        char fname[100], fstat;
        int namefg = 0, c = 0, cnt = 0, id = 0;
        long long int fsize = 0;
        for (int i = strlen(filestr); i >= 0; i--)
        {
            if (filestr[i] == ')')
            {
                id = i;
                break;
            }
        }
        for (int i = 0; i < strlen(filestr); i++)
        {
            if (namefg == 0 && filestr[i] != '(')
                continue;
            if (filestr[i] == '(')
            {
                i++;
                while (i != id)
                    fname[c++] = filestr[i++];
                fname[c] = '\0';
                namefg = 1;
            }
            if (filestr[i] == ' ')
            {
                cnt++;
                continue;
            }
            if (cnt == 1)
                fstat = filestr[i];
            if (cnt == 21)
            {
                while (i < strlen(filestr) && filestr[i] != ' ')
                {
                    fsize *= 10;
                    fsize += (long long int)filestr[i++] - '0';
                }
                break;
            }
        }
        strcpy(file, "");
        sprintf(file, "/proc/%d/exe", pid);
        char path[101], pathmain[201];
        int readval = (int)readlink(file, path, 100);
        path[readval] = '\0';
        if (readval == -1)
            strcpy(pathmain, "(Permission Denied)");
        else
        {
            char chk[201];
            strcpy(chk, shellhome);
            int lenchk = strlen(chk), lendir = strlen(path);
            if (lenchk <= lendir)
            {
                int flag = 0;
                for (int i = 0; i < (lenchk); i++)
                {
                    if (chk[i] != path[i])
                    {
                        flag = 1;
                        break;
                    }
                }
                if (flag == 0)
                {
                    pathmain[0] = '~';
                    for (int i = (lenchk); i <= (lendir); i++)
                    {
                        if (i == (lendir))
                            pathmain[i - lenchk + 1] = '\0';
                        else
                            pathmain[i - lenchk + 1] = path[i];
                    }
                }
                else
                {
                    for (int i = 0; i <= lendir; i++)
                    {
                        if (i != lendir)
                            pathmain[i] = path[i];
                        else
                            pathmain[i] = '\0';
                    }
                }
            }
            else
            {
                for (int i = 0; i <= lendir; i++)
                {
                    if (i != lendir)
                        pathmain[i] = path[i];
                    else
                        pathmain[i] = '\0';
                }
            }
            pathmain[strlen(pathmain)] = '\0';
        }
        printf("pid -- %d\n", pid);
        printf("Process Status -- %c\n", fstat);
        printf("memory -- %lld\n", fsize);
        printf("Executable Path -- %s\n", pathmain);
    }
    fclose(fp);
    return;
}
