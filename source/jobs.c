#include "../head/headers.h"
#include "../head/jobs.h"
#include "../head/process.h"
#include "../head/sighandler.h"

void listjobs()
{
    struct node *q = (struct node *)malloc(sizeof(struct node));
    q = p;
    int cnt = 1;
    char status[10];
    while (q != NULL)
    {
        int pid = q->id;
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
        char filestr[1001], fstat;
        if (fgets(filestr, 1000, fp) != NULL)
        {
            for (int i = strlen(filestr); i >= 0; i--)
            {
                if (filestr[i] == ')')
                {
                    fstat = filestr[i + 2];
                    break;
                }
            }
            fclose(fp);
        }
        if (fstat == 'T')
            strcpy(status, "Stopped");
        else
            strcpy(status, "Running");
        printf("[%d] %s %s [%d]\n", cnt, status, q->name, q->id);
        q = q->next;
        cnt++;
    }
}

void sigjob(char st[])
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
    if (wcnt != 3)
    {
        exerrflag = 1;
        printf("Error: Incorrect number of arguments.\n");
        return;
    }
    int jbno = -1, sgno = 0;
    for (int i = strlen(st); i >= 0; i--)
    {
        if (st[i] >= '0' && st[i] <= '9' && sgno == 0)
        {
            int temp = 0;
            while (st[i] != ' ')
            {
                temp = temp * 10;
                temp += st[i] - '0';
                i--;
            }
            while (temp > 0)
            {
                sgno = sgno * 10;
                sgno += (temp % 10);
                temp /= 10;
            }
            continue;
        }
        if (st[i] >= '0' && st[i] <= '9')
        {
            jbno = st[i] - '0';
            break;
        }
    }
    if (jbno == -1)
    {
        exerrflag = 1;
        printf("Error: Invalid Argument.\n");
        return;
    }
    struct node *q = (struct node *)malloc(sizeof(struct node));
    q = p;
    int cnt = 1;
    while (q != NULL)
    {
        if (cnt == jbno)
        {
            kill(q->id, sgno);
            return;
        }
        q = q->next;
        cnt++;
    }
    if (cnt <= jbno)
    {
        exerrflag = 1;
        printf("Error: Job doesn't exist.\n");
        return;
    }
}

void forejob(char st[])
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
    if (wcnt != 2)
    {
        exerrflag = 1;
        printf("Error: Incorrect number of arguments.\n");
        return;
    }
    int jbno = 0;
    for (int i = 2; i < strlen(st); i++)
    {
        if (st[i] >= '0' && st[i] <= '9')
        {
            jbno *= 10;
            jbno += (st[i] - '0');
        }
    }
    struct node *q = (struct node *)malloc(sizeof(struct node));
    q = p;
    int cnt = 1,stat;
    int shellpid = getpid();
    while (q != NULL)
    {
        if (cnt == jbno)
        {
            signal(SIGTTOU, SIG_IGN);
            signal(SIGTTIN, SIG_IGN);
            if(kill(getpgid(q->id), SIGCONT) < 0)
            {
                printf("Error: Failed to bring process to foreground.\n");
                exerrflag = 1;
                return;
            }
            tcsetpgrp(0, getpgid(q->id));
            waitpid(getpgid(q->id), &stat, WUNTRACED);
            if(!WIFSTOPPED(stat))
            {
                printf("Terminated:\t%s\n",q->name);
                del(q->id);
            }
            tcsetpgrp(0, getpgid(shellpid));
            signal(SIGTTOU, SIG_DFL);
            signal(SIGTTIN, SIG_DFL);
            return;
        }
        q = q->next;
        cnt++;
    }
    if (cnt <= jbno)
    {
        exerrflag = 1;
        printf("Error: Job doesn't exist.\n");
        return;
    }
}

void backjob(char st[])
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
    if (wcnt != 2)
    {
        exerrflag = 1;
        printf("Error: Incorrect number of arguments.\n");
        return;
    }
    int jbno = 0;
    for (int i = 2; i < strlen(st); i++)
    {
        if (st[i] >= '0' && st[i] <= '9')
        {
            jbno *= 10;
            jbno += (st[i] - '0');
        }
    }
    struct node *q = (struct node *)malloc(sizeof(struct node));
    q = p;
    int cnt = 1;
    while (q != NULL)
    {
        if (cnt == jbno)
        {
            if(kill(q->id, SIGCONT) < 0)
            {
                printf("Error: Couldn't send signal.\n");
                exerrflag = 1;
                return;
            }
            return;
        }
        q = q->next;
        cnt++;
    }
    if (cnt <= jbno)
    {
        exerrflag = 1;
        printf("Error: Job doesn't exist.\n");
        return;
    }
}

void killjobs()
{
    exflag = 1;
    while (p != NULL)
        kill(p->id, SIGKILL);
}
