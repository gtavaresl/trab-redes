// C program for the Server Side
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <vector>

// For threading, link with lpthread
#include <pthread.h>

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
pthread_t tid;
pthread_t serverthreads[100];

/**
 * Recebe a mensagem da conexão com o client.
 * 
 * @param network_socket        Socket em que foi estabelecida a conexão
 * @return                      A string com a mensagem recebida  
*/
string recv_string(int network_socket){
	vector<char> buffer(MAX_BUF_LENGTH);
	string rcv;   
	int bytesReceived = 0;
	do {
		bytesReceived = recv(network_socket, &buffer[0], buffer.size(), 0);
		// append string from buffer.
		if ( bytesReceived != -1 )
			rcv.append(buffer.cbegin(), buffer.cend());
	} while ( bytesReceived == MAX_BUF_LENGTH );
    rcv = (string) rcv.c_str();
	return rcv;
}

/**
 * Envia uma mensagem ao client.
 * 
 * @param network_socket        Socket em que foi estabelecida a conexão
 * @param str                   A mensagem a ser enviada
 * @return                      Um inteiro indicando o resultado da função
*/
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

/**
 * Callback para salvar o resultado da query no user_id.
 * 
 * @param data                  Dados recebidos da query      
 * @param argc                  Tamanho da query
 * @param argv                  Argumento em que será salvo os dados
 * @param azColName             Valor da Coluna selecionada
 * @return                      Um inteiro confirmando a operação     
*/
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

/**
 * Callback para salvar o resultado da query na password.
 * 
 * @param data                  Dados recebidos da query      
 * @param argc                  Tamanho da query
 * @param argv                  Argumento em que será salvo os dados
 * @param azColName             Valor da Coluna selecionada
 * @return                      Um inteiro confirmando a operação     
*/
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

/**
 * Callback para exibir os valores armazenados no que foi recebido da query.
 * 
 * @param data                  Dados recebidos da query      
 * @param argc                  Tamanho da query
 * @param argv                  Argumento em que será salvo os dados
 * @param azColName             Valor da Coluna selecionada
 * @return                      Um inteiro confirmando a operação     
*/
int callback_list_passwords(void *data, int argc, char** argv, char** azColName){
    for (int i = 0; i < argc; i++)
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    return 0;
}

/**
 * Consulta o user_id do usuario inserido.
 * 
 * @param db                    Conexão com o banco de dados
 * @param username              String com o username do usuário
 * @param password              String com a senha embaralhada
 * @return                      Retorna string com o id do usuário
*/
string get_client(sqlite3 *db, string username, string password){
    Data_Query *user_id = (Data_Query *)calloc(1, sizeof(Data_Query));
    string sql_query = "SELECT *\nFROM user\nWHERE name like '" + username + "' \nAND password like '" + password + "';";
    int rc = sqlite3_exec(db, sql_query.c_str(), callback_client, user_id, NULL);
    
    return user_id->data;
}

/**
 * Consulta a senha relativa ao website e usuario inserido.
 * 
 * @param db                    Conexão com o banco de dados
 * @param user_id               String com o user_id do usuário
 * @param website               String com a website escolhido
 * @return                      Retorna string com a senha escolhida
*/
string get_password(sqlite3 *db, string user_id, string website){
    Data_Query *password = (Data_Query *)calloc(1, sizeof(Data_Query));
    string sql_query = "SELECT *\nFROM password\nWHERE user_id = '" + user_id + "' \nAND website = '" + website + "';";
    int rc = sqlite3_exec(db, sql_query.c_str(), callback_password, password, NULL);

    return password->data;
}

/**
 * Insere um novo usuario e sua senha.
 * 
 * @param db                    Conexão com o banco de dados
 * @param username              String com o username do usuário
 * @param user_password         String com a senha
 * @return                      Retorna string com a senha escolhida
*/
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

/**
 * Loga ou cadastra o usuário.
 * 
 * @param clientSocket          Socket em que foi estabelecida a conexão
 * @param db                    Conexão com o banco de dados
 * @return                      String com o user_id    
*/
string login(int clientSocket, sqlite3 *db){
    string username;
	string user_password;
	string user_id;

	// Recv Username
	username = recv_string(clientSocket);
    
	cout << "Novo login: Username >" << username << "<" << endl;

	// Recv Password
	user_password = recv_string(clientSocket);
	
	cout << "Password >" << user_password << "<" << endl;

	user_id = get_client(db, username, user_password);

    if(user_id.empty()){
        cout << "Cliente ainda não existe!" << endl;
        user_id = insert_client(db, username, user_password);
    }

	send_string(clientSocket, user_id);
    cout << "[User: " << user_id << "] Login concluído!" << endl;
    return user_id;
}

/**
 * Insere uma nova senha no banco.
 * 
 * @param clientSocket          Socket em que foi estabelecida a conexão
 * @param db                    Conexão com o banco de dados  
 * @param user_id               String com o user_id do usuário
 * @return                      Um inteiro confirmando a operação     
*/
int insert_new_password(int clientSocket, sqlite3 *db, string user_id){
    string website;
    string password;
    char *err_msg;

    website = recv_string(clientSocket);
    cout << "[User: " << user_id << "] Inserir nova senha no website: " << website << endl; 
    password = recv_string(clientSocket);
    cout << "[User: " << user_id << "] Inserir nova senha: " << password << endl; 
    

    string sql_final;

    string sql1 = "INSERT INTO password (user_id, website, password)\nVALUES('";
    string sql2 = "', '";
    string sql3 = "');";

    sql_final = sql1 + user_id + sql2 + website + sql2 + password + sql3;

    int rc = sqlite3_exec(db, sql_final.c_str(), NULL, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        cerr << "[User: " << user_id << "] Erro ao inserir senha!" << endl;
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        
        send_string(clientSocket, "Erro ao inserir senha!");
    }

    cout << "[User: " << user_id << "] Senha incluida com sucesso!" << endl;
    send_string(clientSocket, "Senha incluida com sucesso!");

    return 1;
}

/**
 * Consulta uma senha do usuário.
 * 
 * @param clientSocket          Socket em que foi estabelecida a conexão
 * @param db                    Conexão com o banco de dados  
 * @param user_id               String com o user_id do usuário
 * @return                      Um inteiro confirmando a operação
*/
int client_get_password(int clientSocket, sqlite3 *db, string user_id){
    string website;
    string password;
    website = recv_string(clientSocket);
    password = get_password(db, user_id, website);
    if (password.empty()){
        cout << "[User: " << user_id << "] Website não cadastrado!" << endl;
        send_string(clientSocket, "-1");
    } else {
        cout << "[User: " << user_id << "] Website: " << website << "\tPassword: " << password << endl;
        send_string(clientSocket, password);
    }
    return 1;
}


/**
 * Modifica uma senha do usuário.
 * 
 * @param clientSocket          Socket em que foi estabelecida a conexão
 * @param db                    Conexão com o banco de dados  
 * @param user_id               String com o user_id do usuário
 * @return                      Um inteiro confirmando a operação
*/
int update_password(int clientSocket, sqlite3 *db, string user_id){
    string website;
    string password;
    char *err_msg;

    website = recv_string(clientSocket);
    cout << "Website: " << website << endl;

    password = recv_string(clientSocket);
    cout << "Password: " << password << endl;

    string sql_final;

    string sql1 = "UPDATE password SET password = '";
    string sql2 = "' WHERE user_id = '";
    string sql3 = "' AND website = '";
    string sql4 = "';";

    sql_final = sql1 + password + sql2 + user_id +  sql3 + website + sql4;

    int rc = sqlite3_exec(db, sql_final.c_str(), NULL, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        cerr << "Insert Password Error!" << endl;
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        send_string(clientSocket, "-1");
        
        return -1;
    }

    cout << "[User: " << user_id << "] Senha atualizada com sucesso!" << endl;
    send_string(clientSocket, "1");
    return 1;
}

/**
 * Deleta uma senha do usuário.
 * 
 * @param clientSocket          Socket em que foi estabelecida a conexão
 * @param db                    Conexão com o banco de dados  
 * @param user_id               String com o user_id do usuário
 * @return                      Um inteiro confirmando a operação
*/
int delete_password(int clientSocket, sqlite3 *db, string user_id){
    string website;
    char *err_msg;

    website = recv_string(clientSocket);
    cout << "[User: " << user_id << "] Deletar senha do website: " << website << endl;

    string sql1 = "DELETE FROM password WHERE user_id = '";
    string sql2 = "' AND website = '";
    string sql3 = "';";

    string sql_final = sql1 + user_id + sql2 + website + sql3;    

    int rc = sqlite3_exec(db, sql_final.c_str(), NULL, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        cerr << "Error!" << endl;
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        send_string(clientSocket, "-1");
        return -1;
    }

    cout <<  "[User: " << user_id << "] Senha deletada com sucesso!" << endl;
    send_string(clientSocket, "1");
    return 1;    
}

/**
 * Recebe e seleciona a função desejada pelo usuário.
 * 
 * @param clientSocket          Socket em que foi estabelecida a conexão
 * @param db                    Conexão com o banco de dados  
 * @param user_id               String com o user_id do usuário
 * @return                      Um inteiro para determinar a próxima ação do programa
*/
int menu(int clientSocket, sqlite3 *db, string user_id){        
    int opcao_int;

    opcao_int = stoi(recv_string(clientSocket));
    switch (opcao_int){
        case 1:
            insert_new_password(clientSocket, db, user_id);
            return 1;
            break;
        
        case 2:
            client_get_password(clientSocket, db, user_id);
            return 1;
            break;
        
        case 3:
            update_password(clientSocket, db, user_id);
            return 1;
            break;
        
        case 4:
            delete_password(clientSocket, db, user_id);
            return 1;
            break;
        
        default:
            return 0;
            break;
    }
}

/**
 * Função executada por uma thread com a conexão de um novo client
 * 
 * @param param                 Porta alocada para a conexão
*/
void* serverthread(void *param){
	int clientSocket = *((int*)param);
	int received;
    int loop = 1;
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
    
    while(loop){
        loop = menu(clientSocket, db, user_id);
    }
	
    pthread_exit(NULL);
}

/**
 * Inicia o programa do servidor, aguarda as conexões do client e cria threads para manejá-las.
 * 
 * @return                      Um inteiro para o sistema determinar o funcionamento do programa
*/
int main()
{
	// Initialize variables
	int serverSocket, newSocket;
	struct sockaddr_in serverAddr;
	struct sockaddr_storage serverStorage;

	socklen_t addr_size;

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
		cout << "Server aguardando conexões" << endl;
	else
		cout << "Erro ao inicializar Server Socket" << endl;

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
				cout << "Falha ao criar thread" << endl;
		}
	}

	return 0;
}
