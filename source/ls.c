#include "../head/headers.h"
#include "../head/ls.h"
#include "../head/sighandler.h"

void ls(char st[])
{
    if (st[2] != ' ' && st[2] != '\t' && st[2] != '\n')
    {
        write(1, "Error: Command not Found\n", strlen("Error: Command not Found\n"));
        return;
    }
    int fga = 0, fgl = 0, fg = 0;
    for (int i = 2; i < strlen(st); i++)
    {
        if (st[i] != ' ' && (st[i - 1] == ' ' || st[i-1] == '\t'))
        {
            if (st[i] != '-' && st[i] != '\n')
                fg++;
        }
        if (st[i] == '-' && (st[i - 1] == ' ' || st[i-1] == '\t'))
        {
            i++;
            while (i < strlen(st) && (st[i] != ' ' && st[i] != '\t'))
            {
                if (st[i] == '\n')
                    break;
                if (st[i] == 'a')
                    fga = 1;
                else if (st[i] == 'l')
                    fgl = 1;
                else
                {
                    write(1, "Error: Invalid Attribute\n", strlen("Error: Invalid Attribute\n"));
                    return;
                }
                i++;
            }
        }
    }
    DIR *d;
    struct dirent *dir;
    char *tok = strtok(st, " ");
    tok = strtok(NULL, " ");
    if (tok == NULL || tok[0] == '\n' || fg == 0)
    {
        d = opendir(".");
        if (d)
        {
            if (fgl == 0)
            {
                while ((dir = readdir(d)) != NULL)
                {
                    if (dir->d_name[0] == '.' && fga == 0)
                        continue;
                    printf("%s\t", dir->d_name);
                }
                printf("\n");
            }
            else
            {
                struct stat file;
                long long int size = 0;
                int nameval = 0, groupval = 0, dateval = 0;
                long long int sizeval = 0,flagval = 0;
                while ((dir = readdir(d)) != NULL)
                {
                    if (dir->d_name[0] == '.' && fga == 0)
                        continue;
                    char addr[300];
                    sprintf(addr, "./%s", dir->d_name);
                    if (lstat(addr, &file) == 0)
                    {
                        if (file.st_size > sizeval)
                            sizeval = file.st_size;
                        if (file.st_nlink > flagval)
                            flagval = file.st_nlink;
                        struct passwd *pw = getpwuid(file.st_uid);
                        struct group *gr = getgrgid(file.st_gid);
                        if (strlen(pw->pw_name) > nameval)
                            nameval = strlen(pw->pw_name);
                        if (strlen(gr->gr_name) > groupval)
                            groupval = strlen(gr->gr_name);
                        size += (file.st_blocks*512 + 1023)/1024;
                    }
                }
                int sizecnt = getlen(sizeval), flagcnt = getlen(flagval);
                printf("total %lld\n", (size));
                closedir(d);
                d = opendir(".");
                while ((dir = readdir(d)) != NULL)
                {
                    if (dir->d_name[0] == '.' && fga == 0)
                        continue;
                    char addr[300];
                    sprintf(addr, "./%s", dir->d_name);
                    if (lstat(addr, &file) == 0)
                    {
                        printf((S_ISDIR(file.st_mode)) ? "d" : "-");
                        printf((file.st_mode & S_IRUSR) ? "r" : "-");
                        printf((file.st_mode & S_IWUSR) ? "w" : "-");
                        printf((file.st_mode & S_IXUSR) ? "x" : "-");
                        printf((file.st_mode & S_IRGRP) ? "r" : "-");
                        printf((file.st_mode & S_IWGRP) ? "w" : "-");
                        printf((file.st_mode & S_IXGRP) ? "x" : "-");
                        printf((file.st_mode & S_IROTH) ? "r" : "-");
                        printf((file.st_mode & S_IWOTH) ? "w" : "-");
                        printf((file.st_mode & S_IXOTH) ? "x" : "-");
                        space(flagcnt - getlen((long long int)file.st_nlink) + 1);
                        printf("%ld ", file.st_nlink);
                        struct passwd *pw = getpwuid(file.st_uid);
                        struct group *gr = getgrgid(file.st_gid);
                        printf("%s", pw->pw_name);
                        space(nameval - strlen(pw->pw_name) + 1);
                        printf("%s", gr->gr_name);
                        space(groupval - strlen(gr->gr_name) + 1);
                        space(sizecnt - getlen((long long int)file.st_size));
                        printf("%ld ", file.st_size);
                        char time[50], mon[5];
                        strftime(time, 50, " %d %H:%M", localtime(&file.st_mtime));
                        strftime(mon, 5, "%m", localtime(&file.st_mtime));
                        printmon(mon);
                        printf("%s ", time);
                        printf("%s", dir->d_name);
                    }
                    printf("\n");
                }
            }
            closedir(d);
        }
        else
        {
            perror("Error:");
        }
        return;
    }
    while (tok != NULL)
    {
        if (tok[0] == '-')
        {
            tok = strtok(NULL, " ");
            continue;
        }
        for (int i = 0; i < strlen(tok); i++)
            if (tok[i] == '\n')
                tok[i] = '\0';
        char add[200];
        if (tok[0] == '~')
        {
            sprintf(add, "%s", shellhome);
            for (int i = 1; i < strlen(tok); i++)
                sprintf(add, "%s%c", add, tok[i]);
        }
        else
            strcpy(add, tok);
        d = opendir(add);
        if (d)
        {
            if (fg > 1)
                printf("\n%s:\n", tok);
            if (fgl == 0)
            {
                while ((dir = readdir(d)) != NULL)
                {
                    if (dir->d_name[0] == '.' && fga == 0)
                        continue;
                    printf("%s  ", dir->d_name);
                }
                printf("\n");
            }
            else
            {
                struct stat file;
                long long int size = 0;
                int nameval = 0, groupval = 0, dateval = 0;
                long long int sizeval  = 0, flagval = 0;
                while ((dir = readdir(d)) != NULL)
                {
                    if (dir->d_name[0] == '.' && fga == 0)
                        continue;
                    char addr[300];
                    sprintf(addr, "%s/%s", add,dir->d_name);
                    if (lstat(addr, &file) == 0)
                    {
                        if (file.st_size > sizeval)
                            sizeval = file.st_size;
                        if (file.st_nlink > flagval)
                            flagval = file.st_nlink;
                        struct passwd *pw = getpwuid(file.st_uid);
                        struct group *gr = getgrgid(file.st_gid);
                        if (strlen(pw->pw_name) > nameval)
                            nameval = strlen(pw->pw_name);
                        if (strlen(gr->gr_name) > groupval)
                            groupval = strlen(gr->gr_name);
                        size += file.st_size;
                    }
                }
                int sizecnt = getlen(sizeval), flagcnt = getlen(flagval);
                printf("total %lld\n", size / 1000);
                closedir(d);
                d = opendir(add);
                while ((dir = readdir(d)) != NULL)
                {
                    if (dir->d_name[0] == '.' && fga == 0)
                        continue;
                    char addr[300];
                    sprintf(addr, "%s/%s", add, dir->d_name);
                    if (lstat(addr, &file) == 0)
                    {
                        printf((S_ISDIR(file.st_mode)) ? "d" : "-");
                        printf((file.st_mode & S_IRUSR) ? "r" : "-");
                        printf((file.st_mode & S_IWUSR) ? "w" : "-");
                        printf((file.st_mode & S_IXUSR) ? "x" : "-");
                        printf((file.st_mode & S_IRGRP) ? "r" : "-");
                        printf((file.st_mode & S_IWGRP) ? "w" : "-");
                        printf((file.st_mode & S_IXGRP) ? "x" : "-");
                        printf((file.st_mode & S_IROTH) ? "r" : "-");
                        printf((file.st_mode & S_IWOTH) ? "w" : "-");
                        printf((file.st_mode & S_IXOTH) ? "x" : "-");
                        space(flagcnt - getlen((long long int)file.st_nlink) + 1);
                        printf("%ld ", file.st_nlink);
                        struct passwd *pw = getpwuid(file.st_uid);
                        struct group *gr = getgrgid(file.st_gid);
                        printf("%s", pw->pw_name);
                        space(nameval - strlen(pw->pw_name) + 1);
                        printf("%s", gr->gr_name);
                        space(groupval - strlen(gr->gr_name) + 1);
                        space(sizecnt - getlen((long long int)file.st_size));
                        printf("%ld ", file.st_size);
                        char time[50], mon[5];
                        strftime(time, 50, " %d %H:%M", localtime(&file.st_mtime));
                        strftime(mon, 5, "%m", localtime(&file.st_mtime));
                        printmon(mon);
                        printf("%s ", time);
                        printf("%s", dir->d_name);
                    }
                    printf("\n");
                }
            }
            closedir(d);
        }
        else
        {
            perror("Error:");
        }
        tok = strtok(NULL, " ");
    }
}

int getlen(long long int n)
{
    if(n == 0)
        return 1;
    int val = 0;
    while (n > 0)
    {
        n /= 10;
        val++;
    }
    return val;
}

void space(int n)
{
    while (n > 0)
    {
        n--;
        printf(" ");
    }
}

void printmon(char s[5])
{
    if (strncmp(s, "01", 2) == 0)
        printf("Jan");
    else if (strncmp(s, "02", 2) == 0)
        printf("Feb");
    else if (strncmp(s, "03", 2) == 0)
        printf("Mar");
    else if (strncmp(s, "04", 2) == 0)
        printf("Apr");
    else if (strncmp(s, "05", 2) == 0)
        printf("May");
    else if (strncmp(s, "06", 2) == 0)
        printf("Jun");
    else if (strncmp(s, "07", 2) == 0)
        printf("Jul");
    else if (strncmp(s, "08", 2) == 0)
        printf("Aug");
    else if (strncmp(s, "09", 2) == 0)
        printf("Sep");
    else if (strncmp(s, "10", 2) == 0)
        printf("Oct");
    else if (strncmp(s, "11", 2) == 0)
        printf("Nov");
    else
        printf("Dec");
}