#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <terminals.h>

void writer1(void *);
void writer2(void *);
void writer3(void *);
void writer4(void *);
void writer5(void *);

char string1[] = "abcdefghijklmnopqrstuvwxyz\n";
int length1 = sizeof(string1) - 1;

char string2[] = "?123456789\n";
int length2 = sizeof(string2) - 1;

char string3[] = "qwertyuiopasdfghjklzxcvbnm\n";
int length3 = sizeof(string3) - 1;

char string4[] = "11111111111111111\n";
int length4 = sizeof(string4) - 1;

char string5[] = "00000000000000000\n";
int length5 = sizeof(string5) - 1;


int
main(int argc, char **argv)
{
    InitTerminalDriver();
    InitTerminal(1);

    if (argc > 1) HardwareOutputSpeed(1, atoi(argv[1]));
    if (argc > 2) HardwareInputSpeed(1, atoi(argv[2]));

    ThreadCreate(writer1, NULL);
    ThreadCreate(writer2, NULL);
    ThreadCreate(writer3, NULL);
    ThreadCreate(writer4, NULL);
    ThreadCreate(writer5, NULL);


    ThreadWaitAll();

    exit(0);
}

void
writer1(void *arg)
{
    int status;
    while (1) {
        status = WriteTerminal(1, string1, length1);
        if (status != length1)
            fprintf(stderr, "Error: writer1 status = %d, length1 = %d\n",
                    status, length1);
    }
}

void
writer2(void *arg)
{
    int status;
    while (1) {
        status = WriteTerminal(1, string2, length2);
        if (status != length2)
            fprintf(stderr, "Error: writer2 status = %d, length2 = %d\n",
                    status, length2);
    }
}

void
writer3(void *arg)
{
    int status;
    while (1) {
        status = WriteTerminal(1, string3, length3);
        if (status != length3)
            fprintf(stderr, "Error: writer3 status = %d, length3 = %d\n",
                    status, length3);
    }
}

void
writer4(void *arg)
{
    int status;
    while (1) {
        status = WriteTerminal(1, string4, length4);
        if (status != length4)
            fprintf(stderr, "Error: writer4 status = %d, length4 = %d\n",
                    status, length4);
    }
}

void
writer5(void *arg)
{
    int status;

    while (1) {
        status = WriteTerminal(1, string5, length5);
        if (status != length5)
            fprintf(stderr, "Error: writer5 status = %d, length5 = %d\n",
                    status, length5);
    }
}
