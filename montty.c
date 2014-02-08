#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <threads.h>
#include <hardware.h>

/* Condition variables to lock */
static cond_id_t writer[MAX_NUM_TERMINALS];
static cond_id_t writing[MAX_NUM_TERMINALS];

/* Keep track of the number of writers */
int num_writers = 0;

/* Counter for the number of characters in echo_buf that needs to be echoed */
int echo_count = 0;

/* Buffer for the echo characters */
char echo_buf[10]; // is 10 good enough??

/* Boolean to check whether the echo came from ReceiveInterrupt or not */
bool decrement_echo_count = true;

/* Counter for the number of characters in WriteTerminal buf */
int writeT_buf_count = 0;
int writeT_buf_length = 0;

/* Pointer for the buffer in WriteTerminal */
char *writeT_buf;

/*
 * 
 */
extern
int WriteTerminal(int term, char *buf, int buflen)
{
	Declare_Monitor_Entry_Procedure();

	/* 
	 * Check if you can enter. That is, if there isn't anyone 
	 * else writing on the same terminal.
	 */
	while (num_writers > 0)
		CondWait(writer[term]);
	num_writers++;

	/* Output to the screen */
	writeT_buf = buf;
	writeT_buf_count = buflen;
	writeT_buf_length = buflen;

	WriteDataRegister(term, writeT_buf[0]);

	/* Wait until writing is done */
	CondWait(writing[term]);
	num_writers--;
	CondSignal(writer[term]);

	return buflen;
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
	writer[term] = CondCreate();
	writing[term] = CondCreate();
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
	if (num_writers == 0) {
		if (echo_count == 1) {
			WriteDataRegister(term, echo_buf[0]);
			decrement_echo_count = true;
		}
	} else {
		if (echo_count == 1) {
			decrement_echo_count = false;
		}
	}

}

/*
 * Called by the hardware once each character is written to the 
 * display.
 */
extern
void TransmitInterrupt(int term)
{
	Declare_Monitor_Entry_Procedure();
	int i;

	printf("echo_count = %d, writeT_buf_count = %d, num_writers = %d\n", 
		echo_count, writeT_buf_count, num_writers);
	fflush(stdout);

	if (echo_count > 0) {
		if (decrement_echo_count) {
			echo_count--;
			/* Move the buffer back */
			strcpy(echo_buf, &echo_buf[1]);
		}
	}

	/* Keep echoing as long as there is something to echo */
	if (echo_count > 0) {
		WriteDataRegister(term, echo_buf[0]);
		decrement_echo_count = true;

	/* Write Terminal stuff */
	} else if (writeT_buf_count > 0) {
		writeT_buf_count--;
	
		/* Keep writing as long as there is something to write */
		if (writeT_buf_count > 0) {
			i = writeT_buf_length - writeT_buf_count;
			WriteDataRegister(term, writeT_buf[i]);
		}
		/* else the output is done */
		else {
			CondSignal(writing);
		}
	}
	
}


















