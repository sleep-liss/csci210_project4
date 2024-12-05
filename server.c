
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

struct message {
	char source[50];
	char target[50]; 
	char msg[200]; // message body
};

void terminate(int sig) {
	printf("Exiting....\n");
	fflush(stdout);
	exit(0);
}

int main() {
	int server;
	int target;
	int dummyfd;
	struct message req;
	signal(SIGPIPE,SIG_IGN);
	signal(SIGINT,terminate);
	server = open("serverFIFO",O_RDONLY);
	dummyfd = open("serverFIFO",O_WRONLY);

	while (1) {
		// TODO:
		// read requests from serverFIFO
		ssize_t bytesRead = read(server, &req, sizeof(req));
		if (bytesRead < 0) {
			perror("Failed to read from serverFIFO");
			continue;
		}
		printf("Received a request from %s to send the message %s to %s.\n",req.source,req.msg,req.target);

		// TODO:
		// open target FIFO and write the whole message struct to the target FIFO
		char targetFIFO[50];
		snprintf(targetFIFO, sizeof(targetFIFO), "%s", req.target);
		target = open(targetFIFO, O_WRONLY);
		if (target < 0) {
			printf("Could not open FIFO for %s\n", req.target);
			continue;
		}
	
		//writes status msg
		int writeStatus = write(target, &req, sizeof(req));
		if (writeStatus < 0) {
			perror("Error writing to target FIFO");
		}
		close(target);
	}
	close(server);
	close(dummyfd);
	return 0;
}

