#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 23 // Porta padrão Telnet

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[1024];

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Erro ao criar o soquete do cliente");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erro ao se conectar ao servidor");
        exit(1);
    }

    printf("\nConectado ao servidor Telnet.\n");

    //Troca de mensagens
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        recv(client_socket, buffer, sizeof(buffer), 0);
        printf("\nResposta do servidor: %s", buffer);

        if (strncmp(buffer, "Conexao encerrada", 17) == 0) {
        break;
        }
        
        memset(buffer, 0, sizeof(buffer));
        printf("\nDigite uma mensagem para o servidor: \n");
        fgets(buffer, sizeof(buffer), stdin);
        send(client_socket, buffer, strlen(buffer), 0);
        
    }

    close(client_socket);
    return 0;
}
