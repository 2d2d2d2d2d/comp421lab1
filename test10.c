/* 
 * Tests for 
 * 1. correct output from TerminalDriverStatistics()
 * 2. concurrent writing and queued writing
 * 3. keyboard stroke and write at the very exact moment.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <threads.h>
#include <hardware.h>
#include <terminals.h>

void writer0(void *);
void writer1(void *);
void writer2(void *);
void writer3(void *);
void getStatistics(void *);

char string1[] = "abcdefghijklmnopqrstuvwxyz\n";
int length1 = sizeof(string1) - 1;

char string2[] = "0123456789\n";
int length2 = sizeof(string2) - 1;

struct termstat stats[MAX_NUM_TERMINALS];

int
main(int argc, char **argv)
{
    InitTerminalDriver();
    InitTerminal(0);
    InitTerminal(1);
    InitTerminal(2);

    if (argc > 1) {
        HardwareOutputSpeed(0, atoi(argv[1]));
        HardwareOutputSpeed(1, atoi(argv[1]));
        HardwareOutputSpeed(2, atoi(argv[1]));
    }
    if (argc > 2){
        HardwareInputSpeed(0, atoi(argv[2]));
        HardwareInputSpeed(1, atoi(argv[2]));
        HardwareInputSpeed(2, atoi(argv[2]));
    }

    ThreadCreate(writer0, NULL);
    ThreadCreate(writer1, NULL);
    ThreadCreate(writer2, NULL);
    ThreadCreate(writer3, NULL);
    ThreadCreate(getStatistics, NULL);

    ThreadWaitAll();

    exit(0);
}

void
getStatistics(void *arg)
{
    int i;
    while (1) {
        sleep(1);
        TerminalDriverStatistics(stats);
        printf("Got Stats\n");
        fflush(stdout);
        for (i = 0; i < MAX_NUM_TERMINALS; i++) {
            printf("term %d = {tty_in: %d, tty_out: %d, user_in: %d, user_out: %d}\n", i, stats[i].tty_in, stats[i].tty_out ,stats[i].user_in, stats[i].user_out);
            fflush(stdout);
        }
    }

}

void
writer0(void *arg)
{
    while (1) {
        ;
    }
}

void
writer1(void *arg)
{
    int status1, status2;
    status1 = WriteTerminal(1, string1, length1);
    status2 = WriteTerminal(1, string1, length1);

    while (1)
        ;
}

void
writer2(void *arg)
{
    char buf1[10], buf2[10];

    ReadTerminal(2, buf1, 10);
    ReadTerminal(2, buf2, 10);
    WriteTerminal(2, string1, length1);

    while (1)
        ;
}

void
writer3(void *arg)
{
    int status1, status2;
    status1 = WriteTerminal(1, string2, length2);
    status2 = WriteTerminal(1, string2, length2);

    while (1)
        ;
}
