#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <threads.h>
#include <hardware.h>

/* Condition variables to lock */
static cond_id_t writer[MAX_NUM_TERMINALS];
static cond_id_t writing[MAX_NUM_TERMINALS];
static cond_id_t reader[MAX_NUM_TERMINALS];
static cond_id_t toRead[MAX_NUM_TERMINALS];
static const int ECHO_BUF_SIZE = 1024;
static const int INPUT_BUF_SIZE = 4096;


/* Counter for the number of characters in echo_buf that needs to be echoed */
int echo_count[MAX_NUM_TERMINALS];

/* Buffer for the echo characters */
char echo_buf[MAX_NUM_TERMINALS][1024];

/* Counter for echo buffer writing and reading index */
int echo_buf_write_index[MAX_NUM_TERMINALS];
int echo_buf_read_index[MAX_NUM_TERMINALS];

/* Boolean to check whether the echo came from ReceiveInterrupt or not */
bool decrement_echo_count[MAX_NUM_TERMINALS];

/* Keep track of the number of writers */
int num_writers[MAX_NUM_TERMINALS];

/* Counter for the number of characters in WriteTerminal buf */
int writeT_buf_count[MAX_NUM_TERMINALS];
int writeT_buf_length[MAX_NUM_TERMINALS];

/* Pointer for the buffer in WriteTerminal */
char *writeT_buf[MAX_NUM_TERMINALS];

/* Keep track of the number of readers */
int num_readers[MAX_NUM_TERMINALS];

/* Counter for the number of input that are readable */
int num_readable_input[MAX_NUM_TERMINALS];

/* Counter for input buffer writing and reading index */
int input_buf_write_index[MAX_NUM_TERMINALS];
int input_buf_read_index[MAX_NUM_TERMINALS];

/* Buffer for the input characters */
char input_buf[MAX_NUM_TERMINALS][4096];

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
	char c;
	int i;

	/* 
	 * Check if you can enter. That is, if there isn't anyone 
	 * else reading on the same terminal.
	 */
	while (num_readers[term] > 0)
		CondWait(reader[term]);
	num_readers[term]++;

	/* Wait until we can read */
	while (num_readable_input[term] == 0)
		CondWait(toRead[term]);
	num_readable_input[term]--;

	/* Read from input */
	for (i = 0; i < buflen;) {
		c = input_buf[term][input_buf_read_index[term]];
		input_buf_read_index[term] = (input_buf_read_index[term] + 1) % INPUT_BUF_SIZE;
		strncat(buf, &c, 1);
		i++;
		if ('\n' == c)
			break;
	}
	
	num_readers[term]--;
	CondSignal(reader[term]);

	return i;
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
	reader[term] = CondCreate();
	toRead[term] = CondCreate();
	num_writers[term] = 0;
	echo_count[term] = 0;
	echo_buf_write_index[term] = 0;
	echo_buf_read_index[term] = 0;
	decrement_echo_count[term] = true;
	writeT_buf_count[term] = 0;
	writeT_buf_length[term] = 0;
	num_readers[term] = 0;
	num_readable_input[term] = 0;
	input_buf_write_index[term] = 0;
	input_buf_read_index[term] = 0;

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

	/* Echo buf update */
	echo_buf[term][echo_buf_write_index[term]] = c;
	echo_buf_write_index[term] = (echo_buf_write_index[term] + 1) % ECHO_BUF_SIZE;
	echo_count[term]++;

	/* Input buf update */
	if ('\r' == c) {
		num_readable_input[term]++;
		c = '\n';
		CondSignal(toRead[term]);
	}
	input_buf[term][input_buf_write_index[term]] = c;
	input_buf_write_index[term] = (input_buf_write_index[term] + 1) % INPUT_BUF_SIZE;

	/* If this is the first character, then start the echo */
	if (num_writers[term] == 0) {
		if (echo_count[term] == 1) {
			WriteDataRegister(term, echo_buf[term][echo_buf_read_index[term]]);
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

	/* Something to write from echo */
	if (echo_count[term] > 0) {
		/* decrement count if and only if the WirteDataRegister was written from echo */
		if (decrement_echo_count[term]) {
			echo_count[term]--;
			echo_buf_read_index[term] = (echo_buf_read_index[term] + 1) % ECHO_BUF_SIZE;
		}
	}

	/* Keep echoing as long as there is something to echo */
	if (echo_count[term] > 0) {
		WriteDataRegister(term, echo_buf[term][echo_buf_read_index[term]]);
		decrement_echo_count[term] = true;

	/* WriteTerminal stuff */
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


















