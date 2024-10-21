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

#ifndef MIN
#define MIN(x, y) ((x < y) ? x : y)
#endif

typedef unsigned char BYTE;

/*
void Hexline(char* data, int size)
{
    int i = 0;

    for (i = 0; i < MIN(8, size); i++)
        printf("%02x ", (BYTE)data[i]);

    if (size > 8)
    {
        printf(" ");
        for (i = 8; i < size; i++)
            printf("%02x ", (BYTE)data[i]);
    }

    if (size < 16)
    {
        i = ((16 * 3) + 1) - (size * 3);
        if (size > 8) i--;
        printf("%*c", i, ' ');
    }

    for (i = 0; i < size; i++)
    {
        if (data[i] > 0x20 && data[i] < 0x7f)
            printf("%c", data[i]);
        else
            printf(".");
    }

    return;
}

void Hexdump(char* data, int size)
{
    int offset = 0;
    char* dataPtr = data;

    while (size > 16)
    {
        printf("%08x: ", offset);
        Hexline(dataPtr, 16);
        printf("\n");
        dataPtr += 16;
        offset += 16;
        size -= 16;
    }
    printf("%08x: ", offset);
    Hexline(dataPtr, size);
    printf("\n");

    return;
}
*/

void Hexline(char* data, int size, int skip_first_column, int skip_third_column)
{
    int i = 0;

    // Print the hex representation for the first 8 bytes
    for (i = 0; i < MIN(8, size); i++)
        printf("%02x ", (unsigned char)data[i]);

    // Print a space and the next 8 bytes, if more than 8 bytes
    if (size > 8)
    {
        printf(" ");
        for (i = 8; i < size; i++)
            printf("%02x ", (unsigned char)data[i]);
    }

    // Adjust spacing if less than 16 bytes to align ASCII column
    if (size < 16 && !skip_third_column)
    {
        i = ((16 * 3) + 1) - (size * 3);
        if (size > 8) i--;
        printf("%*c", i, ' ');
    }

    // Print the ASCII representation of the data if not skipping the third column
    if (!skip_third_column)
    {
        for (i = 0; i < size; i++)
        {
            if (data[i] > 0x20 && data[i] < 0x7f)
                printf("%c", data[i]);
            else
                printf(".");
        }
    }

    return;
}

void Hexdump(char* data, int size, int skip_first_column, int skip_third_column)
{
    int offset = 0;
    char* dataPtr = data;

    // Loop over data, 16 bytes at a time
    while (size > 16)
    {
        // Print offset column if not skipping the first column
        if (!skip_first_column)
            printf("%08x: ", offset);

        // Call Hexline to print the hex and ASCII representation
        Hexline(dataPtr, 16, skip_first_column, skip_third_column);
        printf("\n");

        dataPtr += 16;
        offset += 16;
        size -= 16;
    }

    // Print final line
    if (!skip_first_column)
        printf("%08x: ", offset);
    
    Hexline(dataPtr, size, skip_first_column, skip_third_column);
    printf("\n");

    return;
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

    Hexdump(buf, numread, skip_first_column, skip_third_column);

    printf("\n");
    free(buf);
    fclose(fp);
    return 0;
}

