#include "../head/prompt.h"
#include "../head/headers.h"
#include "../head/sighandler.h"

void prompt()
{
    char name[300];
    char usr[41], sys[41], dir[201], dirmain[201];
    getlogin_r(usr, 40);
    gethostname(sys, 40);
    getcwd(dir, 200);
    char chk[200];
    strcpy(chk, shellhome);
    int lenchk = strlen(chk), lendir = strlen(dir);
    if (lenchk <= lendir)
    {
        int flag = 0;
        for (int i = 0; i < (lenchk); i++)
        {
            if (chk[i] != dir[i])
            {
                flag = 1;
                break;
            }
        }
        if (flag == 0)
        {
            dirmain[0] = '~';
            for (int i = (lenchk); i <= (lendir); i++)
            {
                if (i == (lendir))
                    dirmain[i - lenchk + 1] = '\0';
                else
                    dirmain[i - lenchk + 1] = dir[i];
            }
        }
        else
        {
            for (int i = 0; i <= lendir; i++)
            {
                if (i != lendir)
                    dirmain[i] = dir[i];
                else
                    dirmain[i] = '\0';
            }
        }
    }
    else
    {
        for (int i = 0; i <= lendir; i++)
        {
            if (i != lendir)
                dirmain[i] = dir[i];
            else
                dirmain[i] = '\0';
        }
    }
    if (exerrflag == 0)
        sprintf(name, ":')<%s@%s: %s> ", usr, sys, dirmain);
    else
        sprintf(name, ":'(<%s@%s: %s> ", usr, sys, dirmain);    
    write(1, name, strlen(name));
}
