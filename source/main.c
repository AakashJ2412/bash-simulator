#include "../head/prompt.h"
#include "../head/headers.h"
#include "../head/echo.h"
#include "../head/cd.h"
#include "../head/ls.h"
#include "../head/pinfo.h"
#include "../head/process.h"
#include "../head/history.h"
#include "../head/env.h"
#include "../head/jobs.h"
#include "../head/sighandler.h"

extern int errno;
void operation(char[]);

int main()
{
    printf("\e[1;1H\e[2J\n");
    rootpid = getpid();
    strcpy(prevdir, "NULL_DIR");
    signal(SIGCHLD, sig_handler);
    signal(SIGINT, sigc_handler);
    signal(SIGTSTP, sigz_handler);
    getcwd(shellhome, 200);
    exflag = 0, llcnt = 0,exerrflag = 0;
    fgobj.id = -1;
    strcpy(fgobj.name, "");
    prompt();
    char str[101], tok2[101], histstr[201];
    while (1)
    {
        if (fgets(str, 100, stdin) == 0)
        {
            printf("\n");
            process("quit ");
        }
        else
        {
            int spf = 0, c = 0, d = 0, spfp = 0, pcnt = 0;
            for (int i = 0; i < strlen(str); i++)
            {
                if (str[i] == ';' || str[i] == '\n')
                {
                    tok2[c] = ' ', tok2[c + 1] = '\0';
                    if (str[0] == '\n' || tok2[1] == '\0')
                        continue;
                    strcpy(histstr, tok2);
                    save_history(histstr);
                    char pipcmd[100][200];
                    for (int j = 0; j <= strlen(tok2); j++)
                    {
                        if (tok2[j] == '|' || tok2[j] == '\0')
                        {
                            pipcmd[pcnt][d] = ' ', pipcmd[pcnt][d + 1] = '\0';
                            spfp = 0, d = 0;
                            pcnt++;
                            continue;
                        }
                        if ((tok2[j] == ' ' || tok2[j] == '\t') && spfp == 0)
                            continue;
                        spfp = 1;
                        pipcmd[pcnt][d++] = tok2[j];
                    }
                    if (pcnt == 1)
                    {
                        operation(pipcmd[0]);
                        strcpy(tok2, "");
                        spf = 0, c = 0, pcnt = 0;
                        continue;
                    }
                    int ofd[2], nfd[2];
                    for (int j = 0; j < pcnt; j++)
                    {
                        if (j != pcnt - 1)
                        {
                            if (pipe(nfd) < 0)
                            {
                                exerrflag = 1;
                                printf("Error: Bad Pipe\n");
                                break;
                            }
                            if (j == 0)
                            {
                                ofd[0] = nfd[0];
                                ofd[1] = nfd[1];
                            }
                        }
                        int ppid = fork();
                        if (ppid == 0)
                        {
                            if (j != 0)
                            {
                                dup2(ofd[0], 0);
                                close(ofd[0]);
                                close(ofd[1]);
                            }
                            if (j != pcnt - 1)
                            {
                                close(nfd[0]);
                                dup2(nfd[1], 1);
                                close(nfd[1]);
                            }
                            operation(pipcmd[j]);
                            exit(0);
                        }
                        else
                        {
                            if (j != 0)
                            {
                                close(ofd[0]);
                                close(ofd[1]);
                            }
                            if (j != pcnt - 1)
                            {
                                ofd[0] = nfd[0];
                                ofd[1] = nfd[1];
                            }
                            waitpid(ppid, NULL, WUNTRACED);
                        }
                    }
                    close(ofd[0]);
                    close(ofd[1]);
                    strcpy(tok2, "");
                    spf = 0, c = 0, pcnt = 0;
                    continue;
                }
                if ((str[i] == ' ' || str[i] == '\t') && spf == 0)
                    continue;
                spf = 1;
                tok2[c++] = str[i];
            }
            prompt();
            exerrflag = 0;
            strcpy(str, "");
        }
    }
}

void operation(char tok2[])
{
    exerrflag = 0;
    int inflag = -1, outflaga = -1, outflagb = -1;
    for (int k = strlen(tok2); k >= 1; k--)
    {
        if (tok2[k] == '<' && inflag == -1)
        {
            inflag = k;
            continue;
        }
        if (tok2[k] == '>' && tok2[k - 1] != '>' && outflaga == -1)
        {
            outflaga = k;
            continue;
        }
        if (tok2[k] == '>' && tok2[k - 1] == '>' && outflagb == -1)
        {
            outflagb = k;
            continue;
        }
    }
    int readfd, writefd, ogrfd, ogwfd;
    if (inflag != -1)
    {
        char fname[100];
        int spflag = 0, fc = 0;
        for (int k = inflag + 1; k < strlen(tok2); k++)
        {
            if ((tok2[k] == '\t' || tok2[k] == ' ') && spflag == 0)
                continue;
            if ((tok2[k] == '\t' || tok2[k] == ' ') && spflag != 0)
                break;
            spflag = 1;
            fname[fc++] = tok2[k];
        }
        fname[fc] = '\0';
        char add[201];
        if (fname[0] == '~')
        {
            for (int i = 0; i < strlen(tok2); i++)
            {
                fname[i] = fname[i + 1];
            }
            sprintf(add, "%s%s", shellhome, fname);
        }
        else
        {
            strcpy(add, fname);
        }
        readfd = open(add, O_RDONLY);
        if (readfd < 0)
        {
            printf("Error: File doesn't Exist.\n");
            exerrflag = 1;
            return;
        }
        ogrfd = dup(0);
        dup2(readfd, 0);
        close(readfd);
    }
    if (outflaga != -1 || outflagb != -1)
    {
        char fname[100];
        int spflag = 0, fc = 0;
        if (outflagb == -1)
        {
            for (int k = outflaga + 1; k < strlen(tok2); k++)
            {
                if ((tok2[k] == '\t' || tok2[k] == ' ') && spflag == 0)
                    continue;
                if ((tok2[k] == '\t' || tok2[k] == ' ') && spflag != 0)
                    break;
                spflag = 1;
                fname[fc++] = tok2[k];
            }
        }
        else
        {
            for (int k = outflagb + 1; k < strlen(tok2); k++)
            {
                if ((tok2[k] == '\t' || tok2[k] == ' ') && spflag == 0)
                    continue;
                if ((tok2[k] == '\t' || tok2[k] == ' ') && spflag != 0)
                    break;
                spflag = 1;
                fname[fc++] = tok2[k];
            }
        }
        fname[fc] = '\0';
        char add2[201];
        if (fname[0] == '~')
        {
            for (int i = 0; i < strlen(tok2); i++)
            {
                fname[i] = fname[i + 1];
            }
            sprintf(add2, "%s%s", shellhome, fname);
        }
        else
        {
            strcpy(add2, fname);
        }
        if (outflagb != -1)
            writefd = open(add2, O_WRONLY | O_CREAT | O_APPEND, 0644);
        else
            writefd = open(add2, O_WRONLY | O_CREAT, 0644);
        if (writefd < 0)
        {
            printf("Error: File not opened.\n");
            exerrflag = 1;
            return;
        }
        ogwfd = dup(1);
        dup2(writefd, 1);
        close(writefd);
    }
    for (int k = 0; k < strlen(tok2); k++)
    {
        if (tok2[k] == '<' || tok2[k] == '>')
        {
            tok2[k] = ' ';
            tok2[k + 1] = '\0';
            break;
        }
    }
    if (strncmp(tok2, "echo", 4) == 0)
    {
        echo(tok2);
    }
    else if (strncmp(tok2, "pwd", 3) == 0)
    {
        char dirpwd[202];
        getcwd(dirpwd, 200);
        sprintf(dirpwd, "%s\n", dirpwd);
        write(1, dirpwd, strlen(dirpwd));
    }
    else if (strncmp(tok2, "cd", 2) == 0)
    {
        cd(tok2);
    }
    else if (strncmp(tok2, "ls", 2) == 0)
    {
        ls(tok2);
    }
    else if (strncmp(tok2, "pinfo", 5) == 0)
    {
        pinfo(tok2);
    }
    else if (strncmp(tok2, "history", 7) == 0)
    {
        history(tok2);
    }
    else if (strncmp(tok2, "setenv", 6) == 0)
    {
        setenvar(tok2);
    }
    else if (strncmp(tok2, "unsetenv", 8) == 0)
    {
        unsetenvar(tok2);
    }
    else if (strncmp(tok2, "jobs", 4) == 0)
    {
        listjobs();
    }
    else if (strncmp(tok2, "kjob", 4) == 0)
    {
        sigjob(tok2);
    }
    else if (strncmp(tok2, "fg ", 3) == 0)
    {
        forejob(tok2);
    }
    else if (strncmp(tok2, "bg ", 3) == 0)
    {
        backjob(tok2);
    }
    else if (strncmp(tok2, "overkill", 8) == 0)
    {
        killjobs();
    }
    else
    {
        process(tok2);
    }
    if(inflag != -1)
        dup2(ogrfd,0);
    if(outflaga != -1)
        dup2(ogwfd,1);
}