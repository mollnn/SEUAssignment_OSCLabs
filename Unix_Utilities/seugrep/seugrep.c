#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    char *target = argv[1];

    if (argc < 2)
    {
        fputs("searchterm [file ...]\n", stdout);
        return 1;
    }

    if (argc < 3)
    {
        while (1)
        {
            char *buf = NULL;
            size_t file_size;
            getline(&buf, &file_size, stdin);
            if (feof(stdin))
            {
                break;
            }
            char *find_result = strstr(buf, target);
            if (find_result != NULL)
            {
                fputs(buf, stdout);
            }
            if (buf)
                free(buf);
        }
    }
    else
    {
        for (int i = 2; i < argc; i++)
        {
            FILE *fp = fopen(argv[i], "r");

            if (fp == NULL)
            {
                fputs("cannot open file\n", stdout);
                return 1;
            }

            // fseek(fp, 0, SEEK_END);
            // int file_size = ftell(fp);
            // char *buf = malloc(file_size);
            // fseek(fp, 0, SEEK_SET);

            while (1)
            {
                char *buf = NULL;
                size_t file_size;
                getline(&buf, &file_size, fp);
                if (feof(fp))
                {
                    break;
                }
                char *find_result = strstr(buf, target);
                if (find_result != NULL)
                {
                    fputs(buf, stdout);
                }
                if (buf)
                    free(buf);
            }

            fclose(fp);
        }
    }

    return 0;
}