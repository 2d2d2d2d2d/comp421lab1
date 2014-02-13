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
    int a = InitTerminal(-1);
    int b = InitTerminal(0);
    int c = InitTerminal(1);
    int d = InitTerminal(2);
    int e = InitTerminal(2);
    int f = InitTerminal(5);

    printf("%d %d %d %d %d %d\n", a, b, c, d, e, f);
    fflush(stdout);

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
    int status1, status2, status3;
    char buf1[10];

    status1 = WriteTerminal(50, string1, length1);

    status2 = WriteTerminal(*((int *)arg), string1, length1);

    status3 = ReadTerminal(*((int *)arg), buf1, 5000);
    
    printf("%d %d %d\n", status1, status2, status3);
    fflush(stdout);
}

void
writer2(void *arg)
{
    int status1, status2, status3;
    char buf1[10];

    status1 = WriteTerminal(3, string1, length1);

    status2 = WriteTerminal(*((int *)arg), string2, length2);

    status3 = ReadTerminal(*((int *)arg), buf1, 0);
    
    printf("%d %d %d\n", status1, status2, status3);
    fflush(stdout);
}
