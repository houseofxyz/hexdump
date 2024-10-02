#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define VERSION_MAJOR 1
#define VERSION_MINOR 0

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>  // Required for getopt on Unix-like systems
extern int optind;   // Declare optind explicitly if necessary on some systems
#endif

void help(char *f)
{
    puts("hexdump - output a file's raw data");
    printf("usage: %s [options] <inputfilename>\n", f);
    puts("options:");
    puts("\t-h display this text");
    puts("\t-v display version");
    puts("\t-1 skip the first column (offset)");
    puts("\t-3 skip the third column (ASCII)");
    exit(0);
}

void version(char *f)
{
    printf("%s version %d.%d\n", f, VERSION_MAJOR, VERSION_MINOR);
    exit(0);
}

int GetFileSize(FILE *f)
{
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    fseek(f, 0, SEEK_SET);
    return size;
}

void OutputFormatting(unsigned char *buffer, int bufferSize, int skip_first_column, int skip_third_column)
{
    int i = 0, k = 0, counter = 0;
    char ascii[16] = { 0 };

    for(i = 0; i < bufferSize; i++)
    {
        if (i % 16 == 0)
        {
            if (!skip_first_column)  // Print the offset if not skipped
                printf("\n%08X: ", i);
            else
                printf("\n         ");  // Print spaces to maintain alignment
        }

        if(i % 1 == 0) // 1 space between each byte
            printf(" ");
        if(i % 8 == 0) // add extra space every 8 bytes
            printf(" ");
        printf("%02X", buffer[i]); // Print hex

        ascii[k] = buffer[i];

        if (k == 15)
        {
            if(!skip_third_column) // Only print the ASCII representation if not skipped
            {
                printf("  ");
                counter += 16;
                for (int j = 0; j < 16; j++)
                    if (isprint(ascii[j]) != 0)
                        printf("%c", ascii[j]);
                    else
                        printf(".");
            }
            k = 0;
        }
        else
            k++;
    }

    // Print the remaining characters if any
    if(counter < bufferSize)
    {
        int remaining = bufferSize - counter;

        while(remaining < 16)
        {
            printf(" 00");
            if(remaining == 7)
                printf(" ");
            remaining++;
        }

        remaining = bufferSize - counter;

        if (!skip_third_column) // Only print the ASCII representation if not skipped
        {
            printf("  ");
            for(k = 0; k < remaining; k++)
            {
                if(isprint(ascii[k]))
                    printf("%c", ascii[k]);
                else
                    printf(".");
            }
        }
    }
}

#ifdef _WIN32
void parse_arguments(int argc, char **argv, int *skip_first_column, int *skip_third_column)
{
    for (int i = 1; i < argc; i++)  // Start from 1 as argv[0] is the program name
    {
        if (strcmp(argv[i], "-h") == 0)
        {
            help(argv[0]);
        }
        else if (strcmp(argv[i], "-v") == 0)
        {
            version(argv[0]);
        }
        else if (strcmp(argv[i], "-1") == 0)
        {
            *skip_first_column = 1;
        }
        else if (strcmp(argv[i], "-3") == 0)
        {
            *skip_third_column = 1;
        }
        else
        {
            // The remaining non-option argument is considered as the filename
            argv[1] = argv[i];
            return;
        }
    }
}
#else
void parse_arguments(int argc, char **argv, int *skip_first_column, int *skip_third_column)
{
    int opt = 0;
    while((opt = getopt(argc, argv, "hv13")) != -1 )
    {
        switch(opt)
        {
            case 'h':
                help(argv[0]);
                break;
            case 'v':
                version(argv[0]);
                break;
            case '1':  // Skip the first column (offset)
                *skip_first_column = 1;
                break;
            case '3':  // Skip the third column (ASCII representation)
                *skip_third_column = 1;
                break;
            default:
                puts("Invalid option");
                exit(0);
        }
    }
}
#endif

int main(int argc, char **argv)
{
    int numread, fsize = 0;
    int skip_first_column = 0, skip_third_column = 0;
    FILE *fp = NULL;
    unsigned char *buf = NULL;

    // Parse options first
    parse_arguments(argc, argv, &skip_first_column, &skip_third_column);

    // For Windows: argv[1] contains the filename
    // For Linux/Unix: optind points to the filename
#ifdef _WIN32
    if (argc < 2 || argv[1][0] == '-')
#else
    if (optind >= argc)
#endif
    {
        printf("[-] No input file specified\n");
        exit(1);
    }

    // Open the input file
#ifdef _WIN32
    fp = fopen(argv[1], "rb");
#else
    fp = fopen(argv[optind], "rb");
#endif
    if(!fp)
    {
#ifdef _WIN32
        printf("[-] Cannot open input file %s\n", argv[1]);
#else
        printf("[-] Cannot open input file %s\n", argv[optind]);
#endif
        exit(1);
    }

    fsize = GetFileSize(fp);
#ifdef DEBUG
    printf("[*] File size is: %d\n", fsize);
#endif
    buf = (unsigned char *)malloc(fsize);

    numread = fread(buf, 1, fsize, fp);
#ifdef DEBUG
    printf("[*] Number of bytes read: %d\n", numread);
#endif

    OutputFormatting(buf, numread, skip_first_column, skip_third_column);

    printf("\n");
    free(buf);
    fclose(fp);
    return 0;
}

