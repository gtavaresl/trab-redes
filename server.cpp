// C program for the Server Side
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <vector>

// For threading, link with lpthread
#include <pthread.h>
#include <semaphore.h>

// SQL lib
#include <sqlite3.h>

// inet_addr
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define TRUE 1
#define MAX_BUF_LENGTH 4096

using namespace std;

// Global variables
sem_t x;
pthread_t tid;
pthread_t serverthreads[100];
int clientcount = 0;

string recv_string(int network_socket){
	vector<char> buffer(MAX_BUF_LENGTH);
	string rcv;   
	int bytesReceived = 0;
	do {
		bytesReceived = recv(network_socket, &buffer[0], buffer.size(), 0);
		// append string from buffer.
		if ( bytesReceived == -1 ) { 
			// error
		} else {
			rcv.append( buffer.cbegin(), buffer.cend() );
		}
	} while ( bytesReceived == MAX_BUF_LENGTH );
	return rcv;
}

int send_string(int network_socket, string str)
{
    const char* data_ptr  = str.data();
    size_t data_size = str.size();

    int bytes_sent;

    while (data_size > 0)
    {
        bytes_sent = send(network_socket, data_ptr, data_size, 0);
        if (bytes_sent == -1)
            return -1;

        data_ptr += bytes_sent;
        data_size -= bytes_sent;
    }

    return 1;
}

class Data_Query
{
public:
    char data[MAX_BUF_LENGTH];
    Data_Query();
};

int callback_client(void* data, int argc, char** argv, char** azColName){
    Data_Query *DQ = (Data_Query *) data;
    for (int i = 0; i < argc; i++) {
        if (strcmp(azColName[i], "id") == 0){
            strcpy(DQ->data, argv[i]);
            break;
        }
    }
    return 0;
}

int callback_password(void* data, int argc, char** argv, char** azColName){
    Data_Query *DQ = (Data_Query *) data;
    for (int i = 0; i < argc; i++) {
        if (strcmp(azColName[i], "password") == 0){
            strcpy(DQ->data, argv[i]);
            break;
        }
    }
    return 0;
}

int callback_list_passwords(void *data, int argc, char** argv, char** azColName){
    for (int i = 0; i < argc; i++)
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    return 0;
}

string get_client(sqlite3 *db, string username, string password){
    Data_Query *user_id = (Data_Query *)calloc(1, sizeof(Data_Query));
    string sql_query = "SELECT *\nFROM user\nWHERE name = '" + username + "' \nAND password = '" + password + "';";
    int rc = sqlite3_exec(db, sql_query.c_str(), callback_client, user_id, NULL);
    
    return user_id->data;
}

//FALTA TRATAR O ERRO DO WEBSITE INSERIDO NAO EXISTIR NO DB
string get_password(sqlite3 *db, string user_id, string website){
    Data_Query *password = (Data_Query *)calloc(1, sizeof(Data_Query));
    string sql_query = "SELECT *\nFROM password\nWHERE user_id = '" + user_id + "' \nAND website = '" + website + "';";
    int rc = sqlite3_exec(db, sql_query.c_str(), callback_password, password, NULL);

    return password->data;
}

// Insere novo cliente na tabela user
string insert_client(sqlite3 *db, string username, string user_password){
    char *err_msg;

    string sql_final;
    string sql1 = "INSERT INTO user (name, password)\nVALUES('";
    string sql2 = "', '";
    string sql3 = "');";

    sql_final = sql1 + username + sql2 + user_password + sql3;

    int rc = sqlite3_exec(db, sql_final.c_str(), NULL, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        cerr << "Error ao inserir" << endl;
        
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        
        return "-1";
    }
    cout << "Usuário incluido com sucesso!" << endl;

    string user_id = get_client(db, username, user_password);
    return user_id;
}

string login(int clientSocket, sqlite3 *db){
    string username;
	string user_password;
	string user_id;
	// Recv Username
	username = recv_string(clientSocket);
	
	cout << "\n Username >" << username << "<\n" << endl;

	// Recv Password
	user_password = recv_string(clientSocket);
	
	cout << "\n Userpass >" << user_password << "<\n" << endl;
	
	user_id = get_client(db, username, user_password);

    if(user_id.empty()){
        cout << "Cliente ainda não existe!" << endl;
        user_id = insert_client(db, username, user_password);
    }

	send_string(clientSocket, user_id);
    cout << "Login concluído!" << endl;
    return user_id;
}

// Thread Server
void* serverthread(void *param){
	int clientSocket = *((int*)param);
	int received;
	string user_id;
	sqlite3 *db;
    char *err_msg = 0;

    cout << "Novo usuário conectado!" << endl;
    int rc = sqlite3_open("test.db", &db);
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        pthread_exit(NULL);
    }

	user_id = login(clientSocket, db);

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
