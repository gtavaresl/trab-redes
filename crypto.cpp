#include<iostream>
#include<string.h>

using namespace std;

// Implementação da cifra de cesar

string rotate(string msg, int shift){
    for (int i = 0; i < msg.length(); i++)
    {
        if (msg[i] >= 'a' && msg[i] <= 'z')
            msg[i] = 'a' + (msg[i] - 'a' + shift) % 26;
        else if (msg[i] >= 'A' && msg[i] <= 'Z')
            msg[i] = 'A' + (msg[i] - 'A' + shift) % 26;
    }

    return msg;
}

string encode(string msg, int shift){
    return rotate(msg, shift);
}

string decode(string msg, int shift){
    return rotate(msg, 26 - shift);
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