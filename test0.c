#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <terminals.h>

int
main(int argc, char **argv)
{
    InitTerminalDriver();
    InitTerminal(1);

    while (1)
    	;

    exit(0);
}
