#include <stdio.h>      /* for snprintf */
#include "csapp.h"
#include "calc.h"

/* buffer size for reading lines of input from user */
#define LINEBUF_SIZE 1024

//description above implemenation
int chat_with_client(struct Calc *calc, int infd, int outfd);

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("Error: invalid command line arguments\n");
		return 1;
	}

	int server_fd = Open_listenfd(argv[1]);
	struct Calc *calc = calc_create();
	int keep_going = 1;

	while (keep_going) {
		int client_fd = Accept(server_fd, NULL, NULL);
		if (client_fd > 0) {
			keep_going = chat_with_client(calc, client_fd, client_fd);
			close(client_fd);
		}
	}

	close(server_fd);
	calc_destroy(calc);
	return 0;
}

/*
 * Purpose: read input from client and write output to client
 * Parameters:
 * 		calc - a pointer to a Calc struct
 *      infd - (integer) input file descriptor
 *      outfd - (integer) output file descriptor
 * Returns:
 * 		int, whether or not to continue (1) or end (0)
 */
int chat_with_client(struct Calc *calc, int infd, int outfd) {
	rio_t in;
	char linebuf[LINEBUF_SIZE];

	/* wrap standard input (which is file descriptor 0) */
	rio_readinitb(&in, infd);

	/*
	 * Read lines of input, evaluate them as calculator expressions,
	 * and (if evaluation was successful) print the result of each
	 * expression.  Quit when "quit" command is received.
	 */
	int done = 0;
	while (!done) {
		ssize_t n = rio_readlineb(&in, linebuf, LINEBUF_SIZE);
		if (n <= 0) {
			/* error or end of input */
			done = 1;
		} else if (strcmp(linebuf, "quit\n") == 0 || strcmp(linebuf, "quit\r\n") == 0) {
			/* quit command */
			done = 1;
		} else if (strcmp(linebuf, "shutdown\n") == 0 || strcmp(linebuf, "shutdown\r\n") == 0) {
			return 0;
		} else {
			/* process input line */
			int result;
			if (calc_eval(calc, linebuf, &result) == 0) {
				/* expression couldn't be evaluated */
				rio_writen(outfd, "Error\n", 6);
			} else {
				/* output result */
				int len = snprintf(linebuf, LINEBUF_SIZE, "%d\n", result);
				if (len < LINEBUF_SIZE) {
					rio_writen(outfd, linebuf, len);
				}
			}
		}
	}
	return 1;
}
