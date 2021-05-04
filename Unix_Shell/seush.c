#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <wait.h>
#include <string.h>
#include <signal.h>

char ws[] = " \t\r\n\v";

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
    while (1)
    {
        char *av[99];
        for (int i = 0; i < 99; i++)
            av[i] = NULL;
        int ac;
        int pid;
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
                strcat(pathenv, av[i]);
                if (i + 1 < ac)
                    strcat(pathenv, ":");
            }
        }
        else if ((pid = fork()) == 0)
        {
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
                    execv(path, av);
                    pathitem = strtok(NULL, ":");
                }
                strcpy(pathitem, ".");
                if (!flag)
                {
                    strcpy(path, pathitem);
                    strcat(path, "/");
                    strcat(path, av[0]);
                    execv(path, av);
                    pathitem = strtok(NULL, ":");
                }
            }
            throw_error_end();
        }
        wait(0);
    }

    return 0;
}