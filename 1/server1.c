#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>

#define PORT 12345

int main(){
    int server_sock, client_sock;
    struct sockaddr_in server, client;
    socklen_t client_size;
    char message[1024], reply[] = "Hello";

    // Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock == -1){
        perror("Could not create socket");
        return 1;
    }
    printf("Server socket created.\n");

    // Server address info
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to IP and Port
    if(bind(server_sock, (struct sockaddr*)&server, sizeof(server)) < 0){
        perror("Bind failed");
        close(server_sock);
        return 1;
    }
    printf("Bind successful. Listening on port %d...\n", PORT);

    // Listen for clients
    if(listen(server_sock, 1) < 0){
        perror("Listen failed.");
        close(server_sock);
        return 1;
    }

    // Accept client connection
    client_size = sizeof(client);
    client_sock = accept(server_sock, (struct sockaddr*)&client, &client_size);
    if(client_sock < 0){
        perror("Accept failed.");
        close(server_sock);
        return 1;
    }
    printf("Client connected.\n");

    // Receive message from client
    if(recv(client_sock, message, sizeof(message), 0)< 0){
        perror("Receive failed.");
    }
    else{
        printf("Client says: %s\n", message);
    }

    // Send reply
    if(send(client_sock, reply, strlen(reply) + 1, 0) < 0){
        perror("Send failed.");
    }
    else{
        printf("Sent reply: %s\n", reply);
    }

    // Close sockets
    close(client_sock);
    close(server_sock);
    printf("Connection closed.\n");

    return 0;
}