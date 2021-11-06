# trab-redes
Usa a lib pthreads -> requer rodar em linux

P/ compilar: g++ -o {nome_exec} server.cpp -pthread -lsqlite3
             g++ -o {nome_exec} client.cpp -pthread
             
Fluxograma:  

             instalar: sudo apt-get install -y libsqlite3-dev
                       sudo apt-get install sqlite3
                       
             rodar:    g++ -o {nome_exec} fluxograma.cpp -lsqlite3
                       ./{nome_exec}

Caso não tenha como visualizar a tabela, esse site funciona: https://inloop.github.io/sqlite-viewer/


Criptografia Hash:


            instalar: sudo apt-get install libssl-dev   (biblioteca openssl)
            
            Rodar: gcc -o sha256 sha256.c -lcrypto
                   ./sha256 <arquivo>
