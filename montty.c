#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <threads.h>
#include <hardware.h>

/* Condition variables to lock */
static cond_id_t writer[MAX_NUM_TERMINALS];
static cond_id_t writing[MAX_NUM_TERMINALS];

/* Keep track of the number of writers */
int num_writers[MAX_NUM_TERMINALS];

/* Counter for the number of characters in echo_buf that needs to be echoed */
int echo_count[MAX_NUM_TERMINALS];

/* Buffer for the echo characters */
char echo_buf[MAX_NUM_TERMINALS][10]; // is 10 good enough??

/* Boolean to check whether the echo came from ReceiveInterrupt or not */
bool decrement_echo_count[MAX_NUM_TERMINALS];

/* Counter for the number of characters in WriteTerminal buf */
int writeT_buf_count[MAX_NUM_TERMINALS];
int writeT_buf_length[MAX_NUM_TERMINALS];

/* Pointer for the buffer in WriteTerminal */
char *writeT_buf[MAX_NUM_TERMINALS];

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
	while (num_writers[term] > 0)
		CondWait(writer[term]);
	num_writers[term]++;

	/* Output to the screen */
	writeT_buf[term] = buf;
	writeT_buf_count[term] = buflen;
	writeT_buf_length[term] = buflen;

	WriteDataRegister(term, writeT_buf[term][0]);

	/* Wait until writing is done */
	CondWait(writing[term]);
	num_writers[term]--;
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
	num_writers[term] = 0;
	echo_count[term] = 0;
	decrement_echo_count[term] = true;
	writeT_buf_count[term] = 0;
	writeT_buf_length[term] = 0;
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
	strncat(echo_buf[term], &c, 1);
	echo_count[term]++;

	/* If this is the first character, then start the echo */
	if (num_writers[term] == 0) {
		if (echo_count[term] == 1) {
			WriteDataRegister(term, echo_buf[term][0]);
			decrement_echo_count[term] = true;
		}
	} else {
		if (echo_count[term] == 1) {
			decrement_echo_count[term] = false;
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

	printf("terminal %d: echo_count = %d, writeT_buf_count = %d, num_writers = %d\n", 
		term, echo_count[term], writeT_buf_count[term], num_writers[term]);
	fflush(stdout);

	if (echo_count[term] > 0) {
		if (decrement_echo_count[term]) {
			echo_count[term]--;
			/* Move the buffer back */
			strcpy(echo_buf[term], &echo_buf[term][1]);
		}
	}

	/* Keep echoing as long as there is something to echo */
	if (echo_count[term] > 0) {
		WriteDataRegister(term, echo_buf[term][0]);
		decrement_echo_count[term] = true;

	/* Write Terminal stuff */
	} else if (writeT_buf_count[term] > 0) {
		writeT_buf_count[term]--;
	
		/* Keep writing as long as there is something to write */
		if (writeT_buf_count[term] > 0) {
			i = writeT_buf_length[term] - writeT_buf_count[term];
			WriteDataRegister(term, writeT_buf[term][i]);
		}
		/* else the output is done */
		else {
			CondSignal(writing[term]);
		}
	}
	
}


















