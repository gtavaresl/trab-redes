// C program for the Server Side

// inet_addr
#include <arpa/inet.h>

// For threading, link with lpthread
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_CHAR 256

// Global variables
sem_t x;
pthread_t tid;
pthread_t serverthreads[100];
int clientcount = 0;

// Thread Server
void* serverthread(void *param){
	int clientSocket = *((int*)param);
	int received;
	char response[MAX_CHAR];

	// Lock the semaphore
	sem_wait(&x);
	clientcount++;
	// Unlock the semaphore
	sem_post(&x);


	received = recv(clientSocket, response, MAX_CHAR, 0);
	
	response[received-1] = '\0';
	printf("\n Client login >%s<\n", response);
	
	// Lock the semaphore
	sem_wait(&x);
	clientcount--;
	
	// Unlock the semaphore
	sem_post(&x);


	pthread_exit(NULL);
}

// Driver Code
int main()
{
	// Initialize variables
	int serverSocket, newSocket;
	struct sockaddr_in serverAddr;
	struct sockaddr_storage serverStorage;

	socklen_t addr_size;
	sem_init(&x, 0, 1);

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8989);
	memset(&serverAddr.sin_zero,0,sizeof(serverAddr.sin_zero));

	// Bind the socket to the
	// address and port number.
	bind(serverSocket,
		(struct sockaddr*)&serverAddr,
		sizeof(serverAddr));

	// Listen on the socket,
	// with 40 max connection
	// requests queued
	if (listen(serverSocket, 50) == 0)
		printf("Listening\n");
	else
		printf("Error\n");

	// Array for thread
	pthread_t tid[60];

	int i = 0;

	while (1) {
		addr_size = sizeof(serverStorage);

		// Extract the first
		// connection in the queue
		newSocket = accept(serverSocket,
						(struct sockaddr*)&serverStorage,
						&addr_size);
		if (pthread_create(&serverthreads[i++], NULL,
							serverthread, &newSocket)!= 0){
				// Error in creating thread
				printf("Failed to create thread\n");
		}
	}

	return 0;
}
