#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#define MAX_CHAR 256

using namespace std;

class Data_Query
{
public:
    char data[MAX_CHAR];
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
    Data_Query *user_id = (Data_Query *)malloc(sizeof(Data_Query));
    strcpy(user_id->data, "-1");
    string sql_query = "SELECT *\nFROM user\nWHERE name = '" + username + "' \nAND password = '" + password + "';";
    int rc = sqlite3_exec(db, sql_query.c_str(), callback_client, user_id, NULL);
    return user_id->data;
}

string get_password(sqlite3 *db, string user_id, string website){
    Data_Query *password = (Data_Query *)malloc(sizeof(Data_Query));
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

int insert_new_password(sqlite3 *db, string user_id){
    string website;
    string password;
    char *err_msg;

    cout << "Digite nome do website: " << endl;
    getline(cin, website);

    cout << "Digite sua senha: " << endl;
    getline(cin, password);

    string sql_final;

    string sql1 = "INSERT INTO password (user_id, website, password)\nVALUES('";
    string sql2 = "', '";
    string sql3 = "');";

    sql_final = sql1 + user_id + sql2 + website + sql2 + password + sql3;

    int rc = sqlite3_exec(db, sql_final.c_str(), NULL, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        cerr << "Insert Password Error!" << endl;
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        
        return -1;
    }

    cout << "Senha incluida com sucesso!" << endl;
    return 1;
}

string login(sqlite3 *db){
    string username;
    string user_password;
    
    cout << "Digite seu nome de usuário: ";
    getline(cin, username);

    cout << "Digite sua senha: ";
    getline(cin, user_password);
    
    string user_id = get_client(db, username, user_password);
    if(strcmp(user_id.c_str(), "-1") == 0){
        cout << "Cliente ainda não existe!" << endl;
        user_id = insert_client(db, username, user_password);
    }
    cout << "Login concluído!" << endl;
    return user_id;
}

int main(){
    
    sqlite3 *db;
    char *err_msg = 0;
    
    int rc = sqlite3_open("test.db", &db);
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return 1;
    }
    
    string user_id = login(db);
    //get_client(db, "Teste 1", "123456");
    //insert_client(db);
    insert_new_password(db, user_id);
    //cout << "User: " << user_id << "\nPassword: " << get_password(db, "1", "google.com") << endl;

    sqlite3_close(db);
    
    return 0;
}