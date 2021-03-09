#include <stdio.h>

int main(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        FILE *fp = fopen(argv[i], "r");

        char buf[512];

        if (fp == NULL)
        {
            fputs("cannot open file\n", stdout);
            return 1;
        }

        while (1)
        {
            fgets(buf, 512, fp);
            if (feof(fp))
            {
                break;
            }
            fputs(buf, stdout);
        }
    }

    return 0;
}