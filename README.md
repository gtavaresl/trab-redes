# trab-redes
Cliente/Servidor:

              Usa a lib pthreads -> requer rodar em linux

                            instalar: sudo apt-get install -y libsqlite3-dev
                                   sudo apt-get install sqlite3

              P/ compilar: g++ -o {nome_exec} server.cpp -pthread -lsqlite3
                     g++ -o {nome_exec} client.cpp

              P/ Executar: ./{nome_exec}

Fluxograma:

             instalar: sudo apt-get install -y libsqlite3-dev
                       sudo apt-get install sqlite3
                       
             rodar:    g++ -o {nome_exec} fluxograma.cpp -lsqlite3
                       ./{nome_exec}

Caso não tenha como visualizar a tabela, esse site funciona: https://inloop.github.io/sqlite-viewer/

Criptografia - Cifra de Cesar:

            Rodar: g++ -o {nome_exec} crypto.cpp
                   ./{nome_exec} <arquivo>
