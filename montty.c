#include <stdio.h>
#include <threads.h>
#include <hardware.h>

/* Counter for the number of characters in echo_buf that needs to be echoed */
int echo_count = 0;

/* Buffer for the echo characters */
char echo_buf[10]; // is 10 good enough??


/*
 * 
 */
extern
int WriteTerminal(int term, char *buf, int buflen)
{
	Declare_Monitor_Entry_Procedure();
	return 0;
}

/*
 * 
 */
extern
int ReadTerminal(int term, char *buf, int buflen)
{
	Declare_Monitor_Entry_Procedure();
	return 0;
}

/*
 * 
 */
extern
int InitTerminal(int term)
{
	Declare_Monitor_Entry_Procedure();
	return InitHardware(term);
}

/*
 * 
 */
extern
int InitTerminalDriver()
{
	Declare_Monitor_Entry_Procedure();
	return 0;
}

/*
 * 
 */
extern
int TerminalDriverStatistics(struct termstat *stats)
{
	Declare_Monitor_Entry_Procedure();
	return 0;
}

/*
 * Called by the hardware once each character typed on the keyboard is ready.
 * It will concatnate the character into the buffer, increment the count
 * and start writing to the terminal.
 */
extern
void ReceiveInterrupt(int term)
{
	Declare_Monitor_Entry_Procedure();

	char c = ReadDataRegister(term);
	strncat(echo_buf, &c, 1);
	echo_count++;

	/* If this is the first character, then start the echo */
	if (echo_count == 1)
		WriteDataRegister(term, echo_buf[0]);
}

/*
 * Called by the hardware once each character is written to the 
 * display.
 */
extern
void TransmitInterrupt(int term)
{
	Declare_Monitor_Entry_Procedure();

	echo_count--;

	/* Move the buffer back */
	strcpy(echo_buf, &echo_buf[1]);

	/* Keep echoing as long as there is something to echo */
	if (echo_count > 0)
		WriteDataRegister(term, echo_buf[0]);
}


















