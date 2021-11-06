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
	send_string(network_socket, username);

    cout << "Digite sua senha: ";
    getline(cin, user_password);
    
	send_string(network_socket, user_password);
	
	user_id = recv_string(network_socket);
    cout << "Login concluído!" << endl;
	cout << "User id: " << user_id << endl;

    return user_id;
}

int insert_new_password(int network_socket, string user_id){
    string website;
    string password;
	string response;

    cout << "Digite nome do website: " << endl;
    getline(cin, website);
	send_string(network_socket, website);

    cout << "Digite sua senha: " << endl;
    getline(cin, password);
	send_string(network_socket, website);

	response = recv_string(network_socket);

    cout << response << endl;
    return 1;
}

string get_password(int network_socket){
    string website;
    string password;
	
	cout << "Digite o website:" << endl;
	getline(cin, website);
	send_string(network_socket, website);
	
	password = recv_string(network_socket);
	if (strcmp(password.c_str(), "-1") == 0){
		cout << "Website não cadastrado!" << endl;
	} else {
		cout << "Sua senha do " << website << " é " << password << endl;
	}

}

//Altera uma senha existente
int update_password(int network_socket){
    string website;
    string password;
    int response;

    cout << "Digite nome do website: " << endl;
    getline(cin, website);
	send_string(network_socket, website);

    cout << "Digite sua nova senha: " << endl;
    getline(cin, password);
	send_string(network_socket, password);

	response = stoi(recv_string(network_socket));
	if (response == 1)
    	cout << "Senha atualizada com sucesso!" << endl;
	else
		cout << "Erro ao atualizar senha!" << endl;

    return response;
}

//Deleta uma senha no banco de dados
int delete_password(int network_socket){
    string website;
    int response;

    cout << "Digite nome do website: " << endl;
    getline(cin, website);
	send_string(network_socket, website);

	response = stoi(recv_string(network_socket));
	if (response == 1)
    	cout << "Senha deletada com sucesso!" << endl;
	else
		cout << "Erro ao deletar senha!" << endl;


    return 1;
}

int menu(int network_socket, string user_id) {        
    cout << "1 - Guardar uma nova senha;" << endl;
    cout << "2 - Receber uma senha guardada;" << endl;
    cout << "3 - Modificar uma senha guardada;" << endl;
    cout << "4 - Deletar uma senha guardada;" << endl;
    cout << "5 - Sair;" << endl << endl << "Digite a função que deseja:" << endl;

    int opcao_int;
    string opcao;
    getline(cin,opcao);
	send_string(network_socket, opcao);

    opcao_int = stoi(opcao);
    switch (opcao_int) {
		case 1:
			insert_new_password(network_socket, user_id);
			return 1;
			break;
		
		case 2:
			get_password(network_socket);
			return 1;
			break;
		
		case 3:
			update_password(network_socket);
			return 1;
			break;
		
		case 4:
			delete_password(network_socket);
			return 1;
			break;
		
		default:
			return 0;
			break;
    }
}

// Driver Code
int main(){
	string user_id;
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
	
	user_id = login(network_socket);

	// Close the connection
	close(network_socket);
	
}
