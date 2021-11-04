// C program for the Client Side
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

// inet_addr
#include <arpa/inet.h>
#include <unistd.h>

// For threading, link with lpthread
#include <pthread.h>
#include <semaphore.h>

#define TRUE 1
#define MAX_CHAR 256

// Function to send data to
// server socket.
void* clienthread(){

	int network_socket;
	char login[256];

	// Create a stream socket
	network_socket = socket(AF_INET,
							SOCK_STREAM, 0);

	// Initialise port number and address
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(8989);

	memset(&server_address.sin_zero,0,sizeof(server_address.sin_zero));

	// Initiate a socket connection
	int connection_status = connect(network_socket,
									(struct sockaddr*)&server_address,
									sizeof(server_address));

	// Check for connection error
	if (connection_status < 0) {
		puts("Error\n");
		return 0;
	}

	printf("Connection estabilished\n");
	printf("Enter your login: ");

	fgets(login, MAX_CHAR, stdin);

	// Send data to the socket
	send(network_socket, login, strlen(login), 0);

	// Close the connection
	close(network_socket);
	pthread_exit(NULL);

	return 0;
}

// Driver Code
int main(){
	pthread_t tid;

	// Create thread
	pthread_create(&tid, NULL, clienthread, NULL);

	// Suspend execution of
	// calling thread
	pthread_join(tid, NULL);
}
