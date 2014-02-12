#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <terminals.h>

void writer(void *);

char string[] = "\n\n\n1234567890987654321";
int length = sizeof(string) - 1;

int main(int argc, char **argv)
{
    InitTerminalDriver();
    InitTerminal(1);

    if (argc > 1) HardwareOutputSpeed(1, atoi(argv[1]));
    if (argc > 2) HardwareInputSpeed(1, atoi(argv[2]));

    ThreadCreate(writer, NULL);

    while (1)
    	;

    ThreadWaitAll();

    exit(0);
}

void
writer(void *arg)
{
    int status;
    status = WriteTerminal(1, string, length);
    
}