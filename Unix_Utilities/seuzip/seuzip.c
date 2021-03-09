#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fputs("seuzip: file1 [file2 ...]\n", stdout);
        return 1;
    }
    char last_char = -1;
    int last_cnt = 0;
    for (int i = 1; i < argc; i++)
    {
        FILE *fp = fopen(argv[i], "r");

        if (fp == NULL)
        {
            fputs("cannot open file\n", stdout);
            return 1;
        }

        fseek(fp, 0, SEEK_END);
        size_t file_size = ftell(fp);
        char *text = (char *)malloc(file_size + 1);
        rewind(fp);
        fread(text, sizeof(char), file_size, fp);
        text[file_size] = '\0';

        for (int j = 0; j < file_size; j++)
        {
            char cur_char = text[j];
            if (cur_char == last_char)
            {
                ++last_cnt;
            }
            else
            {
                if (last_cnt)
                {
                    fwrite(&last_cnt, 4, 1, stdout);
                    fwrite(&last_char, 1, 1, stdout);
                }
                last_cnt = 1;
                last_char = cur_char;
            }
        }

        fclose(fp);
    }

    if (last_cnt)
    {
        fwrite(&last_cnt, 4, 1, stdout);
        fwrite(&last_char, 1, 1, stdout);
    }
    return 0;
}