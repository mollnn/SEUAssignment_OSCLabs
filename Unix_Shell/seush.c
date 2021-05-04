#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <wait.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>

char ws[] = " \t\r\n\v";

char argu_buffer[32][256];
char redir_filename[256];

void throw_error()
{
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

void throw_error_end()
{
    throw_error();
    exit(1);
}

int split(char *cmd, char **argv)
{
    int i = 0, j = 0, len = 0;

    len = strlen(cmd);
    while (i < len && cmd[i])
    {
        while (i < len && strchr(ws, cmd[i]))
        {
            i++;
        }
        if (i < len)
        {
            argv[j++] = cmd + i;
        }
        while (i < len && !strchr(ws, cmd[i]))
        {
            i++;
        }
        cmd[i++] = 0;
    }
    argv[j] = 0;
    return j;
}

int main(int argc, char *argv[])
{
    char shell_addr[255];
    char current_dir[255];
    getcwd(shell_addr, sizeof(shell_addr));
    getcwd(current_dir, sizeof(current_dir));
    strcat(shell_addr, "/");
    strcat(shell_addr, argv[0]);

    char pathenv[1000] = "/bin";
    char paths[1000];
    char path[255];

    if (argc > 2)
    {
        throw_error_end();
    }
    FILE *fp;
    if (argc == 2)
    {
        fp = fopen(argv[1], "r");
    }

    int cnt = 0;
    while (1)
    {
        // fputs("debug: cmd count = ", stdout);
        // char x[2];
        // x[0] = 0;
        // x[1] = 0;
        // x[0] = '0';
        // x[0] += cnt;
        // fputs(x, stdout);
        // fputs("\n", stdout);
        // cnt++;
        char *av[99];
        int ac;
        int pid;
        int redir_pos = 0, redir_cnt = 0;

        int argu_cnt = 0;
        for (int i = 0; i < 99; i++)
            av[i] = NULL;
        if (argc == 1)
        {
            int length;
            char input_buffer[512];
            memset(input_buffer, 0, sizeof input_buffer);
            char prompt_string[20] = "seush>";
            write(STDOUT_FILENO, prompt_string, strlen(prompt_string));
            length = read(STDIN_FILENO, input_buffer, 512);
            ac = split(input_buffer, av);

            if (length == 0)
                exit(0);
        }
        else
        {
            char *buf = NULL;
            size_t file_size;
            if (feof(fp))
                exit(0);
            getline(&buf, &file_size, fp);

            ac = split(buf, av);

            if (file_size == 0)
                exit(0);
        }

        if (ac > 0 && strcmp(av[0], "exit") == 0)
        {
            if (ac == 1)
                exit(0);
            else
                throw_error();
        }
        else if (ac > 0 && strcmp(av[0], "#!") == 0)
        {
            // just skip
        }
        else if (ac > 0 && strcmp(av[0], "cd") == 0)
        {
            if (ac == 2)
            {
                if (chdir(av[1]))
                    throw_error();
            }
            else
                throw_error();
        }
        else if (ac > 0 && strcmp(av[0], "path") == 0)
        {
            memset(pathenv, 0, sizeof pathenv);
            for (int i = 1; i < ac; i++)
            {
                if (av[i][0] != '/')
                {
                    strcat(pathenv, current_dir);
                    strcat(pathenv, "/");
                }
                strcat(pathenv, av[i]);
                if (i + 1 < ac)
                    strcat(pathenv, ":");
            }
        }
        else if ((pid = fork()) == 0)
        {

            redir_cnt = 0;
            redir_pos = 0;
            for (int i = 1; i < ac; i++)
                if (strcmp(av[i], ">") == 0)
                {
                    redir_cnt++;
                    redir_pos = i;
                }

            if (redir_cnt > 1 || (redir_cnt > 0 && redir_pos + 1 >= ac))
            {
                throw_error_end();
            }
            else if (redir_cnt == 1)
            {
                strcpy(redir_filename, av[redir_pos + 1]);

                for (int i = 0; i < ac; i++)
                    if (i < redir_pos || i > redir_pos + 1)
                        strcpy(argu_buffer[argu_cnt++], av[i]);
                for (int i = 0; i < ac; i++)
                    av[i] = 0;
                ac -= 2;
                for (int i = 0; i < ac; i++)
                    av[i] = argu_buffer[i];

                close(1);
                int fd = open(redir_filename, O_WRONLY | O_CREAT, 0777);
                int copyFd = dup2(1, fd);
            }

            if (av[0][0] == '/')
            {
                execv(av[0], av);
                throw_error_end();
            }
            else
            {
                strcpy(paths, pathenv);
                char *pathitem = strtok(paths, ":");
                int flag = 0;

                while (pathitem)
                {
                    strcpy(path, pathitem);
                    strcat(path, "/");
                    strcat(path, av[0]);
                    int len = strlen(path);
                    if (len > 3 && path[len - 3] == '.' && path[len - 2] == 's' && path[len - 1] == 'h')
                    {
                        int aac = ac + 1;
                        char *aav[99];
                        memset(aav, 0, sizeof aav);
                        aav[0] = malloc(256);
                        strcpy(aav[0], shell_addr);
                        for (int i = 0; i < ac; i++)
                        {
                            aav[i + 1] = malloc(256);
                            strcpy(aav[i + 1], av[i]);
                        }
                        strcpy(aav[1], path);
                        FILE *fp = fopen(path, "r");
                        if (fp)
                        {
                            fclose(fp);
                            // fputs(aav[1], stdout);
                            // fputs(" we re running\n", stdout);
                            execv(aav[0], aav);
                        }
                    }
                    else
                    {
                        execv(path, av);
                    }

                    pathitem = strtok(NULL, ":");
                }
                if (!flag)
                {
                    strcpy(path, ".");
                    strcat(path, "/");
                    strcat(path, av[0]);
                    int len = strlen(path);
                    if (len > 3 && path[len - 3] == '.' && path[len - 2] == 's' && path[len - 1] == 'h')
                    {
                        int aac = ac + 1;
                        char *aav[99];
                        memset(aav, 0, sizeof aav);
                        aav[0] = malloc(256);
                        strcpy(aav[0], shell_addr);
                        for (int i = 0; i < ac; i++)
                        {
                            aav[i + 1] = malloc(256);
                            strcpy(aav[i + 1], av[i]);
                        }
                        strcpy(aav[1], path);
                        FILE *fp = fopen(path, "r");
                        if (fp)
                        {
                            fclose(fp);
                            // fputs(aav[1], stdout);
                            // fputs(" we re running\n", stdout);
                            execv(aav[0], aav);
                        }
                    }
                    else
                    {
                        execv(path, av);
                    }
                }
            }
            throw_error_end();
        }
        else
            wait(0);
    }

    return 0;
}