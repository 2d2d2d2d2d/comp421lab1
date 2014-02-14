#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    ThreadCreate(reader, one);
    ThreadCreate(reader, one);

    while (1)
    	;

    ThreadWaitAll();

    exit(0);
}

void
reader(void *arg)
{
    char buf1[1];
    while (1) {
        ReadTerminal(*((int *)arg), buf1, 10);
        printf("%s\n", buf1);
        fflush(stdout);
        strcpy(buf1, "");
    }

    
}
