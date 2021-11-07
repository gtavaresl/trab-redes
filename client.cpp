// C program for the Client Side
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <vector>
#include <fstream>

// inet_addr
#include <arpa/inet.h>
#include <unistd.h>

#define TRUE 1
#define MAX_BUF_LENGTH 4096

using namespace std;

// Funcoes que manipulam a chave de criptografia
// Le a chave do arquivo key.txt
int load_key(){
    fstream file;
    file.open("key.txt", ios::in);
    if (file.is_open()){
        string key;
        getline(file, key);
        file.close();
        return stoi(key);
    }
    return -1;
}

int generate_key(){
    srand(time(0));
    int key = rand() % 94; // Gerando chave aleatoria
    fstream file;
    file.open("key.txt", ios::out);
    if (file.is_open()){
        file << to_string(key);
        file.close();
        return key;
    }
    return -1;
}

string rotate(string msg, int key){
    string error;
    for (int i = 0; i < msg.length(); i++){
        if (msg[i] < '!' || msg[i] > '~')
            return error;
        msg[i] = '!' + (msg[i] - '!' + key) % 94;
    }

    return msg;
}

string encode(string msg, int key){
    return rotate(msg, key);
}

string decode(string msg, int key){
    return rotate(msg, 94 - key);
}

// Funcoes que enviam e recebem as strings na rede
string recv_string(int network_socket){
	vector<char> buffer(MAX_BUF_LENGTH);
	string rcv;   
	int bytesReceived = 0;
	do {
		bytesReceived = recv(network_socket, &buffer[0], buffer.size(), 0);
		// append string from buffer.
		if ( bytesReceived != -1 )
			rcv.append( buffer.cbegin(), buffer.cend() );
	} while ( bytesReceived == MAX_BUF_LENGTH );
	rcv = (string) rcv.c_str();
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

// Funcoes de acao do cliente na aplicacao
string login(int network_socket, int key){
	string username;
    string user_password;
	string user_id;

    cout << "Digite seu nome de usuário: ";
    getline(cin, username);
	send_string(network_socket, username);

    cout << "Digite sua senha: ";
    getline(cin, user_password);

	string encrypted = encode(user_password, key);
	while (encrypted.empty()){
        cout << "Erro de char nao permitido!" << endl;
        cout << "Digite sua senha: " << endl;
        getline(cin, user_password);
        encrypted = encode(user_password, key);
    }
    
	send_string(network_socket, encrypted);
	
	user_id = recv_string(network_socket);

    cout << "Login concluído!" << endl;
	cout << "User id: " << user_id << endl;

    return user_id;
}

int insert_new_password(int network_socket, string user_id, int key){
    string website;
    string password;
	string response;

    cout << "Digite nome do website: ";
    getline(cin, website);
	send_string(network_socket, website);

    cout << "Digite sua senha: ";
    getline(cin, password);

	string encrypted = encode(password, key);
	while (encrypted.empty()){
        cout << "Erro de char nao permitido!" << endl;
        cout << "Digite sua senha: " << endl;
        getline(cin, password);
        encrypted = encode(password, key);
    }

	send_string(network_socket, encrypted);

	response = recv_string(network_socket);

    cout << response << endl;
    return 1;
}

int get_password(int network_socket, int key){
    string website;
    string password;
	
	cout << "Digite o website: ";
	getline(cin, website);
	send_string(network_socket, website);
	
	password = recv_string(network_socket);
	if (strcmp(password.c_str(), "-1") == 0){
		cout << "Website não cadastrado!" << endl;
	} else {
		password = decode(password, key);
		cout << "Website: " << website << "\tPassword: " << password << endl;
	}
	return 1;
}

//Altera uma senha existente
int update_password(int network_socket, int key){
    string website;
    string password;
    int response;

    cout << "Digite nome do website: ";
    getline(cin, website);
	send_string(network_socket, website);

    cout << "Digite sua nova senha: ";
    getline(cin, password);

	string encrypted = encode(password, key);
	while (encrypted.empty()){
        cout << "Erro de char nao permitido!" << endl;
        cout << "Digite sua nova senha: " << endl;
        getline(cin, password);
        encrypted = encode(password, key);
    }
	send_string(network_socket, encrypted);

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

    cout << "Digite nome do website: ";
    getline(cin, website);
	send_string(network_socket, website);

	response = stoi(recv_string(network_socket));
	if (response == 1)
    	cout << "Senha deletada com sucesso!" << endl;
	else
		cout << "Erro ao deletar senha!" << endl;

    return 1;
}

int menu(int network_socket, string user_id, int key) {        
    cout << "1 - Guardar uma nova senha;" << endl;
    cout << "2 - Receber uma senha guardada;" << endl;
    cout << "3 - Modificar uma senha guardada;" << endl;
    cout << "4 - Deletar uma senha guardada;" << endl;
    cout << "5 - Sair;" << endl << endl << "Digite a função que deseja: ";

    int opcao_int;
    string opcao;
    getline(cin,opcao);
	send_string(network_socket, opcao);

    opcao_int = stoi(opcao);
    switch (opcao_int) {
		case 1:
			insert_new_password(network_socket, user_id, key);
			break;
		
		case 2:
			get_password(network_socket, key);
			break;
		
		case 3:
			update_password(network_socket, key);
			break;
		
		case 4:
			delete_password(network_socket);
			break;
		
		default:
			return 0;
    }
	return 1;
}

// Main code
int main(){
	int key;
	string user_id;
	int network_socket;
	int loop = 1;

    cout << "Carregando chave..." << endl;
    key = load_key();
    if(key == -1){
        cout << "Erro ao carregar arquivo chave..." << endl;
        cout << "Gerando nova chave" << endl;
        key = generate_key();
    } else {
        cout << "Chave carregada com sucesso!" << endl;
    }

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
		cout << "Erro na conexão!" << endl;
		return 0;
	}

	cout << "Conexao estabelecida!" << endl;
	
	user_id = login(network_socket, key);

    while(loop){
        loop = menu(network_socket, user_id, key);
    }

	// Close the connection
	close(network_socket);
	
}
