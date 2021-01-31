#include "../head/headers.h"
#include "../head/history.h"
#include "../head/sighandler.h"

void save_history(char st[])
{
    char filestr[201];
    int c = 0;
    for (int i = 1; i < strlen(st); i++)
    {
        if (st[i] == '\t')
            st[i] = ' ';
        if (st[i] == ' ' && st[i - 1] == ' ')
            continue;
        filestr[c++] = st[i];
        if (i == strlen(st) - 1)
        {
            if (st[i] != '\n')
            {
                st[i + 1] = '\n';
                st[i + 2] = '\0';
            }
            else
                st[i + 1] = '\0';
            break;
        }
    }
    char rpath[1001],wpath[1001];
    sprintf(rpath,"%s/history",shellhome);
    sprintf(wpath,"%s/hist_temp",shellhome);
    FILE *fr = fopen(rpath, "r");
    FILE *fw = fopen(wpath, "w+");
    if (fr == NULL || fw == NULL)
    {
        exerrflag = 1;
        printf("Error obtaining history\n");
        return;
    }
    for (int i = 0; i < strlen(st); i++)
        fputc(st[i], fw);
    char fstr[1001];
    int fcnt = 0;
    while (fgets(fstr, 1000, fr) != NULL && fcnt < 19)
    {
        fcnt++;
        for (int i = 0; i < strlen(fstr); i++)
            fputc(fstr[i], fw);
    }
    fclose(fr);
    fclose(fw);
    int del = remove(rpath);
    if(!del)
        rename(wpath,rpath);
}

void history(char st[])
{
    int val = 0;
    for (int i = 7; i < strlen(st); i++)
    {
        if (st[i] == '\t' || st[i] == ' ' || st[i] == '\n')
            continue;
        if (st[i] >= '0' && st[i] <= '9')
        {
            val = val * 10;
            val += (int)st[i] - '0';
        }
    }
    if (val > 20)
        val = 20;
    if (val == 0)
        val = 10;
    char rpath[1001];
    sprintf(rpath,"%s/history",shellhome);
    FILE *fp = fopen(rpath, "r");
    if (fp == NULL)
        return;
    char filestr[1001];
    while (val--)
    {
        if (fgets(filestr, 1000, fp) != NULL)
        {
            printf("%s", filestr);
            if (filestr[strlen(filestr) - 1] != '\n')
                printf("\n");
        }
    }
    fclose(fp);
}
