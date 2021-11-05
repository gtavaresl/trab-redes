#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>


int main(void) {
    
    sqlite3 *db;
    char *err_msg = 0;
    
    int rc = sqlite3_open("test.db", &db);
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return 1;
    }
    

    //SE POSSÍVEL TRANSFORMAR EM UMA FUNÇÃO
    //Tabela User/Password
    char *nome_tabela = "user";
    char username[50];
    printf("Digite seu nome de usuário: ");
    fgets(username, 50, stdin);
    char user_password[50];
    printf("Digite sua senha: ");
    fgets(user_password, 50, stdin);


    char sql_final[50];

    char *sql1 = "INSERT INTO ";
    char *sql2 = " VALUES('";
    char *sql3 = "', '";
    char *sql4 = "');";



    strcpy(sql_final, sql1);
    strcat(sql_final, nome_tabela);
    strcat(sql_final, sql2);

    strcat(sql_final, username);
    strcat(sql_final, sql3);
    strcat(sql_final, user_password);
    strcat(sql_final, sql4);


//TENTEI FAZER ESSA SEGUNDA TABELA MAS CONFLITA COM O RC (PRECISA ARRUMAR)
 /*   //Tabela Password INSERT INTO password VALUES('hfernandes', 'facebook', 'riodejaneiro');
    char *nome_tabela2 = "password";
    char username2[50];
    printf("Digite seu nome de usuário: ");
    fgets(username2, 50, stdin);
    char sofware[50];
    printf("Digite o software para guardar senha: ");
    fgets(sofware, 50, stdin);
    char sofware_password[50];
    printf("Digite a senha do sofware escolhido: ");
    fgets(sofware, 50, stdin);


    char sql_final_2[50];

    char *sql1_2 = "INSERT INTO ";
    char *sql2_2 = " VALUES('";
    char *sql3_2 = "', '";
    char *sql4_2 = "', '";
    char *sql5_2 = "');";



    strcpy(sql_final_2, sql1_2);
    strcat(sql_final_2, nome_tabela2);
    strcat(sql_final_2, sql2_2);
    strcat(sql_final_2, username2);
    strcat(sql_final_2, sql3_2);
    strcat(sql_final_2, sofware);
    strcat(sql_final_2, sql4_2);
    strcat(sql_final_2, sofware_password);
    strcat(sql_final_2, sql5_2);*/




    rc = sqlite3_exec(db, sql_final, 0, 0, &err_msg);
    //rc = sqlite3_exec(db, sql_final_2, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        
        return 1;
    } 
    
    sqlite3_close(db);
    
    return 0;
}
