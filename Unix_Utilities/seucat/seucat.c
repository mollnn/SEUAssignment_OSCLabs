#include <stdio.h>

int main(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        FILE *fp = fopen(argv[i], "r");

        char tmp;

        if (fp == NULL)
        {
            fputs("cannot open file\n", stdout);
            return 1;
        }

        while (1)
        {
            tmp = fgetc(fp);
            if (feof(fp))
            {
                break;
            }
            fputc(tmp, stdout);
        }
    }

    return 0;
}