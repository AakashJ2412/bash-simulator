#include "../head/headers.h"
#include "../head/echo.h"
#include "../head/sighandler.h"

void echo(char st[])
{
    if (st[4] != ' ' && st[4] != '\t' && st[4] != '\n')
    {
        exerrflag = 1;
        write(1, "Error: Command not Found\n", strlen("Error: Command not Found\n"));
        return;
    }
    int n = strlen(st);
    if (st[n] != '\n')
    {
        st[n] = '\n';
        st[n+1] = '\0';
    }
    int flag = 0, i = 4;
    char out[50000];
    while (i < strlen(st) && (st[i] == ' ' || st[i] == '\t'))
        i++;
    for (i; i < strlen(st); i++)
    {
        if (st[i] == ' ' || st[i] == '\t')
        {
            flag = 1;
            continue;
        }
        else
        {
            if (flag == 1)
            {
                sprintf(out, "%s ", out);
                flag = 0;
            }
            sprintf(out, "%s%c", out, st[i]);
        }
    }
    //   sprintf(out,"%s\n",out);
    write(1, out, strlen(out));
    strcpy(out, "");
}
