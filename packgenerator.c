#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <windows.h>

int checkLine(char *line, char *name)
{
    int i = 0;
    while (*(line + i) != '\t')
    {
        if(*(line + i + 1) != '\t' && *(line+i+1) == '/' && *(line+i+2) == '/')
        {
            while(*(line + i) != '\t') {
                i++;
            }
            return i;
        }
        if (*(line + i) != *(name + i))
            return 0;
        i++;
    }
    return *(name + i) == '\0' ? i : 0;
}

char *getId(char *cardName)
{
    FILE *lookup = fopen("mids.tsv", "r");
    int len = strlen(cardName);
    char *abbreviatedName = malloc(sizeof(char) * (len + 1));
    for (int i = 0; i < len+1; i++)
    {
        if (*(cardName + i) != '(' && *(cardName + i) != '[')
        {
            *(abbreviatedName + i) = *(cardName + i);
        }
        else
        {
            *(abbreviatedName + i - 1) = '\0';
            i = len;
        }
    }
    char *line = malloc(sizeof(char) * 100);
    while (fgets(line, 100, lookup))
    {
        int x = checkLine(line, abbreviatedName);
        if (x != 0)
        {
            return line + x;
        }
    }
    fclose(lookup);
    free(line);
    free(abbreviatedName);
    return "0";
}

int main(int argc, char *argv[])
{
    FILE *fp;
    int packCount = 1;
    int SIZE = 360;
    bool VERBOSE = false;
    bool OPEN_PAGE = false;
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            if (*(argv[i]) != '-' || *(argv[i] + 2) != '\0')
            {
                printf("Invalid arg: %s, use 'generatepacks -h' for help\n", argv[i]);
                exit(EXIT_FAILURE);
            }
            switch (*(1 + argv[i]))
            {
            case 'f':
                i++;
                if (i == argc)
                {
                    printf("Missing arg for -f\n");
                    printf("use 'generatepacks -h' for help\n");
                    exit(EXIT_FAILURE);
                }
                fp = fopen(argv[i], "r");
                if (fp == NULL)
                {
                    printf("%s file is missing or -f option is incorrectly used\n", argv[i]);
                    printf("use 'generatepacks -h' for help\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 't':
                i++;
                if (i == argc)
                {
                    printf("Missing arg for -t\n");
                    printf("use 'generatepacks -h' for help\n");
                    exit(EXIT_FAILURE);
                }
                SIZE = atoi(argv[i]);
                if (SIZE <= 14 || SIZE > 1000)
                {
                    printf("total cube size %s is invalid, please use 'generatepacks -t [CARD COUNT (between 15 and 1,000)]'\n", argv[i]);
                    printf("use 'generatepacks -h' for help\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'v':
                VERBOSE = true;
                break;
            case 'o':
                OPEN_PAGE = true;
                break;
            case 'c':
                i++;
                if (i == argc)
                {
                    printf("Missing arg for -c\n");
                    printf("use 'generatepacks -h' for help\n");
                    exit(EXIT_FAILURE);
                }
                packCount = atoi(argv[i]);
                if (packCount <= 0 || packCount > 36)
                {
                    printf("pack count %s is invalid, please use 'generatepacks -c [PACK COUNT (between 1 and 36)]'\n", argv[i]);
                    printf("use 'generatepacks -h' for help\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'h':
                printf("\n\tJason Doyle's generatepacks utility!\n");
                printf("\n");
                printf("generatepacks [-h] [-v] [-c Pack Count] [-f File] [-t Total Cards]\n");
                printf("\n");
                printf("-h option: help. Will not execute after outputting this\n");
                printf("-v option: verbose option. Include some error detection stuff.\n");
                printf("-c option: Pack count. Default is 1. after -c, include a number between 1 and 36 to specify the number of packs to generate\n");
                printf("-f option: File option. Default is cards.tsv. after -f, specify a file name\n");
                printf("-t option: Total card count. Default is 360. after -t, specify the number of cards\n");
                printf("-o option: Open page. Open an html file with a display of all cards in each pack as images taken from gatherer. Requires presence of mids.tsv file.\n");
                printf("\n");
                printf("File format should be 'tsv' file, as outputted by google spreadsheets: File -> Download.\n");
                printf("The card count should match the number of cards in the tsv file.\n");
                printf("To prevent entries from being counted, have their first character be '~'.\n");
                printf("\n");
                exit(EXIT_SUCCESS);
                break;
            default:
                printf("Invalid arg: %s", argv[i]);
                printf("use 'generatepacks -h' for help\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    if (fp == NULL)
    {
        fp = fopen("cards.tsv", "r");
        if (fp == NULL)
        {
            printf("cards.tsv file is missing, please include file or use -f option'\n");
            exit(EXIT_FAILURE);
        }
    }
    printf("\n");

    srand(time(NULL));
    char **cards = malloc(sizeof(char *) * SIZE);

    char line[1000];
    int iter = 0;
    int wordLen = 0;
    if (VERBOSE)
        printf("Handling file\n");
    while (fgets(line, sizeof(line), fp))
    {
        char *lp = line;
        while (*lp != '\0')
        {
            if (*lp == '\t')
            {
                if (wordLen > 0 && *(lp - wordLen) != '~')
                {
                    if (iter >= SIZE)
                    {
                        printf("hit card count limit, increase card count\n");
                        printf("use 'generatepacks -h' for help\n");
                        exit(EXIT_FAILURE);
                    }
                    cards[iter] = malloc(sizeof(char) * (wordLen + 1));
                    for (int i = 0; i < wordLen; i++)
                    {
                        cards[iter][i] = *(lp - wordLen + i);
                    }
                    cards[iter][wordLen] = '\0';
                    iter++;
                }
                wordLen = 0;
            }
            else
            {
                wordLen++;
            }
            lp += 1;
        }
        if (wordLen > 0 && *(lp - wordLen) != '~')
        {
            if (iter >= SIZE)
            {
                printf("hit card count limit, increase card count\n");
                printf("use 'generatepacks -h' for help\n");
                exit(EXIT_FAILURE);
            }
            cards[iter] = malloc(sizeof(char) * (wordLen + 1));
            for (int i = 0; i < wordLen; i++)
            {
                cards[iter][i] = *(lp - wordLen + i);
            }
            cards[iter][wordLen - 1] = '\0';
            iter++;
        }
        wordLen = 0;
    }
    if (iter < SIZE)
    {
        printf("found less cards (%d) than card count (%d), may experience incorrect or missing cards\n", iter, SIZE);
        printf("use 'generatepacks -h' for help\n");
        printf("\n");
    }
    fclose(fp);

    if (VERBOSE)
        printf("Generating packs (num = %d)\n", packCount);
    int *pack = malloc(sizeof(int) * 15 * packCount);

    for (int i = 0; i < 15 * packCount; i++)
    {
        bool isUnique = false;
        while (isUnique == false)
        {
            isUnique = true;
            pack[i] = rand() % SIZE;
            for (int x = 0; x < i; x++)
            {
                if (pack[x] == pack[i])
                {
                    isUnique = false;
                }
            }
        }
    }
    for (int x = 0; x < packCount; x++)
    {
        printf("pack %d is ", x + 1);
        for (int i = 0; i < 15; i++)
        {
            printf("%s; ", cards[pack[x * 15 + i]]);
            if ((i + 1) % 5 == 0)
            {
                printf("\n");
            }
        }
        printf("\n");
    }
    if (OPEN_PAGE)
    {
        fp = fopen("packs.html", "w");
        fprintf(fp, "<!DOCTYPE html><html><body>");
        for (int i = 0; i < packCount; i++)
        {
            fprintf(fp, "<h2>Pack %d</h2>", i+1);
            for (int x = 0; x < 15; x++)
                fprintf(fp, "<img src=\"https://gatherer.wizards.com/Handlers/Image.ashx?multiverseid=%s&type=card#%s\" alt=\"%s\" width=\"212\" height=\"296\">\n", getId(cards[pack[i * 15 + x]]), cards[pack[i * 15 + x]], cards[pack[i * 15 + x]]);
        }
        fprintf(fp, "</body></html>");
        fclose(fp);
        ShellExecute(NULL, "open", "packs.html",
                NULL, NULL, SW_SHOWNORMAL);
    }
    exit(EXIT_SUCCESS);
}
