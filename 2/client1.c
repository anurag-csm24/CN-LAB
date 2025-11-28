#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>

#define PORT 12345

int main(){
    int sock;
    struct sockaddr_in server;
    char buffer[1024];
    char fruit[50];
    int qty;

    // Create socket
    sock  = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        perror("Socket creation failed.");
        return 1;
    }

    // Server address setup
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("10.0.0.1");

    // Connect to the server
    if(connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0){
        perror("Connection failed.");
        close(sock);
        return 1;
    }

    printf("Connected to Fruit Server!\n");

    // Get user input
    printf("Enter fruit name and quantity (e.g., apple 2): ");
    scanf("%s %d", fruit, &qty);

    // Send request
    sprintf(buffer, "%s %d", fruit, qty);
    send(sock, buffer, strlen(buffer), 0);

    // Receive server response
    int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if(bytes > 0){
        buffer[bytes] = '\0';
        printf("\nServer Response: \n%s", buffer);
    }

    // Receive second message (customer count)
    bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if(bytes > 0){
        buffer[bytes] = '\0';
        printf("%s\n", buffer);
    }

    // Close connection
    close(sock);
    printf("Connection closed.\n");

    return 0;
}