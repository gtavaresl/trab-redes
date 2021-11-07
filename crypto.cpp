#include<iostream>
#include<string.h>

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

int main()
{
    srand(time(0));
    string message;
    cout << "Message: " << endl;
    getline(cin, message);
    int key = rand() % 94;
    
    string encrypted = encode(message, key);
    cout << "Encrypted message:" << encrypted <<endl;
    string decrypted = decode(encrypted, key);
    cout << "Decrypted message:" << decrypted << endl;
    
    return 0;
}