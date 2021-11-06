// C program for the Client Side
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <vector>

// inet_addr
#include <arpa/inet.h>
#include <unistd.h>

#define TRUE 1
#define MAX_BUF_LENGTH 4096

using namespace std;

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

string login(int network_socket){
	string username;
    string user_password;
	string user_id;
    
    cout << "Digite seu nome de usuário: ";
    getline(cin, username);

    cout << "Digite sua senha: ";
    getline(cin, user_password);
    
	send_string(network_socket, username);
	send_string(network_socket, user_password);

    user_id = recv_string(network_socket);
	/*string user_id = get_client(db, username, user_password);
    if(user_id.empty()){
        cout << "Cliente ainda não existe!" << endl;
        user_id = insert_client(db, username, user_password);
    }*/


    cout << "Login concluído!" << endl;
	cout << "User id: " << user_id << endl;
    return user_id;
}

// Driver Code
int main(){
	int network_socket;

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
	
	login(network_socket);

	// Close the connection
	close(network_socket);
	
}
