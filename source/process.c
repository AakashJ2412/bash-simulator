#include "../head/headers.h"
#include "../head/process.h"
#include "../head/prompt.h"
#include "../head/sighandler.h"

void insert(int a, char s[])
{
    llcnt++;
    if (p == NULL)
    {
        struct node *q = (struct node *)malloc(sizeof(struct node));
        q->id = a;
        strcpy(q->name, s);
        q->next = NULL;
        p = q;
    }
    else
    {
        struct node *r = (struct node *)malloc(sizeof(struct node));
        r = p;
        while (r->next != NULL)
            r = r->next;
        struct node *q = (struct node *)malloc(sizeof(struct node));
        q->id = a;
        strcpy(q->name, s);
        q->next = NULL;
        r->next = q;
    }
}

void find(int a)
{
    if (p == NULL)
        return;
    else
    {
        struct node *r = (struct node *)malloc(sizeof(struct node));
        r = p;
        while (r->id != a && r != NULL)
            r = r->next;
        if (exflag == 0)
        {
            if (r == NULL)
            {
                exerrflag = 1;
                printf("[[Invalid PID]]\n");
                return;
            }
            else
                printf("%s", r->name);
        }
        del(a);
    }
}

void del(int a)
{
    llcnt--;
    if (p == NULL)
        return;
    else if (p->id == a)
    {
        p = p->next;
        return;
    }
    struct node *q = (struct node *)malloc(sizeof(struct node));
    struct node *r = (struct node *)malloc(sizeof(struct node));
    q = p;
    r = p->next;
    while (r->id != a)
    {
        r = r->next;
        q = q->next;
        if (r == NULL)
            return;
    }
    q->next = r->next;
}

void process(char st[])
{
    if (strncmp(st, "quit", 4) == 0)
    {
        exflag = 1;
        if (st[4] == '\n' || st[4] == ' ')
        {
            while (p != NULL)
                kill(p->id, SIGKILL);
        }
        exit(0);
    }
    pid_t chlpid;
    int flag = 0, status;
    char str2[200];
    strcpy(str2, st);
    for (int i = strlen(st) - 1; i > 0; i--)
    {
        if (st[i] == ' ' || st[i] == '\t' || st[i] == '\n')
            continue;
        if (st[i] == '&')
            flag = 1;
        break;
    }
    chlpid = fork();
    if (chlpid < 0)
    {
        printf("Error: Could not fork process.\n");
        exerrflag = 1;
        return;
    }
    else if (chlpid == 0)
    {
        for (int i = 0; i < strlen(st); i++)
            if (st[i] == '\t' || st[i] == '\n')
                st[i] = ' ';
        char *args[100];
        char *tok = strtok(st, " ");
        int row = 0;
        while (tok != NULL)
        {
            if (tok[0] == '\n' || tok[0] == '&')
                break;
            args[row] = tok;
            tok = strtok(NULL, " ");
            row++;
        }
        args[row] = NULL;
        int pid = getpid();
        if (flag == 0)
        {
            if (execvp(args[0], args) < 0)
            {
                exerrflag = 1;
                printf("Unknown Command\n");
                exit(1);
            }
            exit(1);
        }
        else
        {
            printf("[%d] %d\n", llcnt + 1, pid);
            setpgid(0, 0);
            if (execvp(args[0], args) < 0)
            {
                exerrflag = 1;
                printf("Unknown Command\n");
                exit(1);
            }
            exit(1);
        }
        return;
    }
    else
    {
        if (flag == 0)
        {
            char nam[200];
            int f = 0;
            for (int i = 0; i < strlen(str2); i++)
                if (str2[i] == '\t')
                    str2[i] = ' ';
            for (int i = 0; i <= strlen(str2); i++)
            {
                if (str2[i] == '&')
                    break;
                if (str2[i] == ' ' && str2[i + 1] == ' ')
                    continue;
                nam[f++] = str2[i];
            }
            nam[f] = '\0';
            fgobj.id = chlpid;
            strcpy(fgobj.name, nam);
            int errstat;
            waitpid(chlpid, &errstat, WUNTRACED);
            if (WEXITSTATUS(errstat))
                exerrflag = 1;
            fgobj.id = -1;
            strcpy(fgobj.name, "");
        }
        else
        {
            char nam[200];
            int f = 0;
            for (int i = 0; i < strlen(str2); i++)
                if (str2[i] == '\t')
                    str2[i] = ' ';
            for (int i = 0; i <= strlen(str2); i++)
            {
                if (str2[i] == '&')
                    break;
                if (str2[i] == ' ' && str2[i + 1] == ' ')
                    continue;
                nam[f++] = str2[i];
            }
            nam[f] = '\0';
            insert(chlpid, nam);
            sleep(0.7);
        }
        return;
    }
}