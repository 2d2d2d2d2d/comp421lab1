/*
 * Test for correct reading
 */

#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <terminals.h>

void reader(void *);

char string[] = "1234567890987654321";
int length = sizeof(string) - 1;

int main(int argc, char **argv)
{
    InitTerminalDriver();
    InitTerminal(1);
    //InitTerminal(2);
    int i = 1;
    //int j = 2;
    void *one = &i;
    //void *two = &j;

    if (argc > 1) HardwareOutputSpeed(1, atoi(argv[1]));
    if (argc > 2) HardwareInputSpeed(1, atoi(argv[2]));
    //if (argc > 1) HardwareOutputSpeed(2, atoi(argv[1]));
    //if (argc > 2) HardwareInputSpeed(2, atoi(argv[2]));

    ThreadCreate(reader, one);
    //ThreadCreate(reader, one);
    //ThreadCreate(reader, two);

    while (1)
    	;

    ThreadWaitAll();

    exit(0);
}

void
reader(void *arg)
{
    int len1, len2, len3, len4;
    char buf1[10];
    char buf2[10];
    char buf3[10];
    char buf4[10];

    len1 = ReadTerminal(*((int *)arg), buf1, 10);
    printf("%d %s\n", len1, buf1);
    fflush(stdout);
    len2 = ReadTerminal(*((int *)arg), buf2, 10);
    printf("%d %s\n", len2, buf2);
    fflush(stdout);
    len3 = ReadTerminal(*((int *)arg), buf3, 10);    
    printf("%d %s\n", len3, buf3);
    fflush(stdout);
    len4 = ReadTerminal(*((int *)arg), buf4, 10);    
    printf("%d %s\n", len4, buf4);
    fflush(stdout);
    
}
