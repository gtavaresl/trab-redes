#include<iostream>
#include<string.h>

using namespace std;

string encryptyfunc(string message, int key) {
    for(int i = 0; i < message.length(); i++)
        message[i] += key;
    return message;
}

string decryptfunc(string message, int key) {

    for(int i = 0; i < message.length(); i++)
        message[i] -= key;
    return message;
}

int main()
{
    srand(time(0));
    string message;
    cout << "Message: " << endl;
    getline(cin, message);
    int key = rand() % 256;
    
    string encrypted = encryptyfunc(message, key);
    cout << "Encrypted message:" << encrypted <<endl;
    string decrypted = decryptfunc(encrypted, key);
    cout << "Decrypted message:" << decrypted << endl;
    
    return 0;
}