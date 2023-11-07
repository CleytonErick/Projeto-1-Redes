#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/resource.h>

#define PORT 23 // Porta padrão Telnet

void *client_handler(void *arg);

int main() {
    int server_fd, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;

    int opt = 1;

    struct rlimit rlp;

    // Obtém o limite atual de descritores de arquivos
    if (getrlimit(RLIMIT_NOFILE, &rlp) == 0) {
        printf("Número máximo de descritores de arquivos (soft limit): %lu\n", rlp.rlim_cur);
        printf("Número máximo de descritores de arquivos (hard limit): %lu\n", rlp.rlim_max);
    } else {
        perror("Erro ao obter o limite de descritores de arquivos");
    }

    // Inicializa o servidor

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Erro ao criar o soquete");
        exit(1);
    }
    printf("Servidor Telnet iniciado...\n");

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt))) {
        perror("Configuracao nao pode ser atribuida");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        perror("Erro ao vincular o soquete");
        exit(1);
    }

    if (listen(server_fd, rlp.rlim_cur) == 0) {
        printf("Aguardando conexões...\n");
    } else {
        perror("Erro ao ouvir conexões");
        exit(1);
    }

    
    while (1){

        client_len = sizeof(client_addr);
        client_socket = accept(server_fd, (struct sockaddr*) &client_addr, &client_len);
        if (client_socket < 0) {
            perror("Erro ao aceitar a conexão do cliente");
            continue;
        }
        printf("\nConexão estabelecida com o cliente %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Cria um novo thread para lidar com o cliente
        pthread_t thread;
        if (pthread_create(&thread, NULL, client_handler, (void *)&client_socket) != 0) {
            perror("Erro ao criar thread para o cliente");
            continue;
        }
        pthread_detach(thread); // Libera recursos automaticamente após o término do thread
    }

    printf("\nServidor Telnet Fechado...\n");
    close(server_fd);
    return 0;
}


void *client_handler(void *arg) {
    int client_socket = *((int *)arg);
    char buffer[1024];

    memset(buffer, 0, sizeof(buffer));
    send(client_socket, "Server Message\n", strlen("Server Message\n"), 0);
    printf("\nMensagem enviada para cliente\n");

    //Troca de mensagens
    while (1){
        recv(client_socket, buffer, sizeof(buffer), 0);
        printf("\nMensagem recebida: %s", buffer);

        if (strncmp(buffer, "sair", 4) == 0) {
        memset(buffer, 0, sizeof(buffer));
        send(client_socket, "Conexao encerrada com cliente\n", strlen("Conexao encerrada com cliente\n"), 0);
        printf("\nConexao encerrada com cliente\n");
        close(client_socket);
        break;
        }

        memset(buffer, 0, sizeof(buffer));
        send(client_socket, "Server Message\n", strlen("Server Message\n"), 0);
        printf("\nMensagem enviada para cliente\n");
    }

    pthread_exit(NULL);
}
