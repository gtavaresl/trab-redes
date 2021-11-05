# trab-redes
Usa a lib pthreads -> requer rodar em linux

P/ compilar: gcc -pthread server.c -o {nome_exec}
             gcc -pthread client.c -o {nome_exec}
             
Fluxograma:  

             instalar: sudo apt-get install -y libsqlite3-dev
                       sudo apt-get install sqlite3
                       
             rodar:    gcc -o version fluxograma.c -lsqlite3
                       ./version

Caso não tenha como visualizar a tabela, esse site funciona: https://inloop.github.io/sqlite-viewer/


Criptografia Hash:
            instalar: sudo apt-get install libssl-dev   (biblioteca openssl)
            
            Rodar: gcc -o sha256 sha256.c -lcrypto
                   ./sha256 <arquivo>
