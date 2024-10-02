#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int GetFileSize(FILE *f)
{
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    fseek(f, 0, SEEK_SET);

    return size;
}

int main(int argc, char **argv) {
    int i = 0, numread, k = 0, counter = 0, fsize = 0;
    FILE *fp = NULL;
    unsigned char *buf = NULL;
    char ascii[16] = { 0 };

    if(argc != 2) {
        printf("[-] Wrong number of command line arguments\n");
        printf("[-] Usage: %s <inputfilename>\n", argv[0]);
        exit(1);
    }

    fp = fopen(argv[1], "rb");
    if(!fp) {
        printf("[-] Cannot open input file %s\n", argv[1]);
        exit(1);
    }

    fsize = GetFileSize(fp);
    printf("[*] File size is: %d\n", fsize);
    buf = (unsigned char *)malloc(fsize);

    numread = fread(buf, 1, fsize, fp);
    printf("[*] Number of bytes read: %d\n", numread);

    for(i = 0; i < numread; i++) {
        if(i % 16 == 0)
            printf("\n%08X: ", i);
        if(i % 1 == 0) // 1 space between each byte
            printf(" ");
        if(i % 8 == 0) // add extra space every 8 bytes
            printf(" ");
        printf("%02X", buf[i]); // if you use lower case 'x' it will display letters in lower case (in hex numbers)

        ascii[k] = buf[i];

        if (k == 15) {
            printf("  ");
            counter += 16;
            for (int j = 0; j < 16; j++)
                if (isprint(ascii[j])!= 0)
                    printf("%c", ascii[j]);
                else
                    printf(".");
            k = 0;
        }
        else
            k++;
    }

    // check difference between printed chars and numread and print the remaining
    if(counter < numread) {
        int remaining = numread - counter;

        while(remaining < 16)
        {
            printf(" 00");
            if(remaining == 7)
                printf(" ");
            remaining++;
        }

        remaining = numread - counter;
        printf("  ");
        for(k = 0; k < remaining; k++)
        {
            if(isprint(ascii[k]))
                printf("%c", ascii[k]);
            else
                printf(".");
        }
    }

    printf("\n");
    return 0;
}
