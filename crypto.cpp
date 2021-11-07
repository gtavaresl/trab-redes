#include<iostream>
#include<string.h>
#include<fstream>

using namespace std;

// Implementação da cifra de cesar p/ os char imprimiveis da tabela ASCII

string rotate(string msg, int shift){
    string error;
    for (int i = 0; i < msg.length(); i++){
        if (msg[i] < '!' || msg[i] > '~')
            return error;
        msg[i] = '!' + (msg[i] - '!' + shift) % 94;
    }

    return msg;
}

string encode(string msg, int shift){
    return rotate(msg, shift);
}

string decode(string msg, int shift){
    return rotate(msg, 94 - shift);
}

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

int main()
{
    int key;
    cout << "Carregando chave..." << endl;
    key = load_key();
    if(key == -1){
        cout << "Erro ao carregar arquivo chave..." << endl;
        cout << "Gerando nova chave" << endl;
        key = generate_key();
    } else {
        cout << "Chave carregada com sucesso!" << endl;
    }
    string message;
    cout << "Message: " << endl;
    getline(cin, message);
    string encrypted = encode(message, key);
    cout << "Encrypted message:" << encrypted <<endl;
    string decrypted = decode(encrypted, key);
    cout << "Decrypted message:" << decrypted << endl;
    
    return 0;
}