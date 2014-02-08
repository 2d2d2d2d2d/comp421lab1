#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <terminals.h>

int
main(int argc, char **argv)
{
    InitTerminalDriver();
    InitTerminal(1);
    InitTerminal(2);

    if (argc > 1) HardwareOutputSpeed(1, atoi(argv[1]));
    if (argc > 2) HardwareInputSpeed(1, atoi(argv[2]));
    if (argc > 1) HardwareOutputSpeed(2, atoi(argv[1]));
    if (argc > 2) HardwareInputSpeed(2, atoi(argv[2]));

    while (1)
    	;

    exit(0);
}
