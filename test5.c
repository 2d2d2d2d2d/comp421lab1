#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <terminals.h>

void writer1(void *);
void writer2(void *);

char string1[] = "abcdefghijklmnopqrstuvwxyz\n";
int length1 = sizeof(string1) - 1;

char string2[] = "0123456789\n";
int length2 = sizeof(string2) - 1;

int
main(int argc, char **argv)
{
    InitTerminalDriver();
    InitTerminal(1);
    InitTerminal(2);
    int i = 1;
    int j = 2;
    void *one = &i;
    void *two = &j;

    if (argc > 1) HardwareOutputSpeed(1, atoi(argv[1]));
    if (argc > 2) HardwareInputSpeed(1, atoi(argv[2]));
    if (argc > 1) HardwareOutputSpeed(2, atoi(argv[1]));
    if (argc > 2) HardwareInputSpeed(2, atoi(argv[2]));

    ThreadCreate(writer1, one);
    ThreadCreate(writer2, one);
    ThreadCreate(writer1, two);
    ThreadCreate(writer2, two);

    ThreadWaitAll();

    exit(0);
}

void
writer1(void *arg)
{
    int status;
    printf("In writer1 with terminal %d\n", *((int *)arg));
    fflush(stdout);
    status = WriteTerminal(*((int *)arg), string1, length1);
    printf("Return from WriteTerminal in writer1\n");
    fflush(stdout);
    if (status != length1)
	fprintf(stderr, "Error: writer1 status = %d, length1 = %d\n",
	    status, length1);
}

void
writer2(void *arg)
{
    int status;
    printf("In writer2 with terminal %d\n", *((int *)arg));
    fflush(stdout);
    status = WriteTerminal(*((int *)arg), string2, length2);
    printf("Return from WriteTerminal in writer2\n");
    fflush(stdout);
    if (status != length2)
	fprintf(stderr, "Error: writer2 status = %d, length2 = %d\n",
	    status, length2);
}
