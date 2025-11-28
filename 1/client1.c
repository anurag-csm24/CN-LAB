#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>

#define PORT 12345

int main(){
    int sock;
    struct sockaddr_in server;
    char message[1024] = "Hi";
    char response[1024];

    // Create a socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1){
        perror("Could not create socket.");
        return 1;
    }
    printf("Socket created.\n");

    // Server information
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("10.0.0.1");

    // Connect to the server
    if(connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0){
        perror("Connection failed.");
        close(sock);
        return 1;
    }
    printf("Connected to server.\n");

    // Send message
    if(send(sock, message, strlen(message) + 1, 0) < 0){
        perror("Send failed");
        close(sock);
        return 1;
    }
    printf("Message sent: %s\n", message);

    // Receive server reply
    if(recv(sock, response, strlen(response), 0) < 0){
        perror("Receive failed");
    }
    else{
        printf("Server replied: %s\n", response);
    }

    // Close socket
    close(sock);
    printf("Connection closed.\n");

    return 0;
}