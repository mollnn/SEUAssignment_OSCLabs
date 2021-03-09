#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fputs("seuunzip: file1 [file2 ...]\n", stdout);
        return 1;
    }
    for (int i = 1; i < argc; i++)
    {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL)
        {
            fputs("cannot open file\n", stdout);
            return 1;
        }
        int new_count;
        char new_char;
        while (1)
        {
            fread(&new_count, 4, 1, fp);
            fread(&new_char, 1, 1, fp);
            if (feof(fp))
            {
                break;
            }
            for (int j = 0; j < new_count; j++)
            {
                fputc(new_char, stdout);
            }
        }
    }
    return 0;
}