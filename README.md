# trab-redes
Usa a lib pthreads -> requer rodar em linux

P/ compilar: gcc -pthread server.c -o {nome_exec}
             gcc -pthread client.c -o {nome_exec}
             
Fluxograma: 
             instalar: sudo apt-get install -y libsqlite3-dev
                       sudo apt-get install sqlite3
             rodar:    gcc -o version fluxograma.c -lsqlite3
                       ./version
