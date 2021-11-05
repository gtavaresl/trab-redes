#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#define MAX_CHAR 256

// Insere novo cliente na tabela user
int insere_cliente(sqlite3 *db){
    char username[MAX_CHAR];
    char user_password[MAX_CHAR];
    char *err_msg = 0;

    printf("Digite seu nome de usuário: ");
    fgets(username, MAX_CHAR, stdin);
    username[strlen(username)-1] = '\0';

    printf("Digite sua senha: ");
    fgets(user_password, MAX_CHAR, stdin);
    user_password[strlen(user_password)-1] = '\0';

    char sql_final[100*MAX_CHAR];

    char *sql1 = "INSERT INTO user (name, password)\nVALUES('";
    char *sql2 = "', '";
    char *sql3 = "');";

    strcpy(sql_final, sql1);
    strcat(sql_final, username);
    strcat(sql_final, sql2);
    strcat(sql_final, user_password);
    strcat(sql_final, sql3);

    printf(">%s<\n", sql_final);

    int rc = sqlite3_exec(db, sql_final, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        
        return -1;
    }
    printf("Usuário incluido com sucesso!\n");

    // Deve retornar o id do usuário
    return 1;
}

int insere_senha(sqlite3 *db, int user_id){
    char website[MAX_CHAR];
    char password[MAX_CHAR];
    char *err_msg = 0;

    printf("Digite nome do website: ");
    fgets(website, MAX_CHAR, stdin);
    website[strlen(website)-1] = '\0';

    printf("Digite sua senha: ");
    fgets(password, MAX_CHAR, stdin);
    password[strlen(password)-1] = '\0';

    char sql_final[100*MAX_CHAR];

    char *sql1 = "INSERT INTO password (user_id, website, password)\nVALUES('";
    char *sql2 = "', '";
    char *sql3 = "');";

    //char str_user_id[256];
    char *str_user_id = "1";
    //itoa(user_id, str_user_id, 10);  

    strcpy(sql_final, sql1);
    strcat(sql_final, str_user_id);
    strcat(sql_final, sql2);
    strcat(sql_final, website);
    strcat(sql_final, sql2);
    strcat(sql_final, password);
    strcat(sql_final, sql3);

    printf(">%s<\n", sql_final);

    int rc = sqlite3_exec(db, sql_final, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        
        return -1;
    }
    printf("Senha incluida com sucesso!\n");
    return 1;
}

int main() {
    
    sqlite3 *db;
    char *err_msg = 0;
    
    int rc = sqlite3_open("test.db", &db);
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return 1;
    }
    
    //insere_cliente(db);
    insere_senha(db, 1);

    sqlite3_close(db);
    
    return 0;
}
