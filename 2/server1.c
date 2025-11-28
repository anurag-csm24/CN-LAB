#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<time.h>

#define PORT 12345
#define MAX_FRUITS 5
#define MAX_CLIENTS 10

struct Fruit{
    char name[30];
    int quantity;
};

struct Client{
    /* data */
    char ip[INET_ADDRSTRLEN];
    int port;
};

int client_count = 0;
struct Client clients[MAX_CLIENTS];

// Checks if client is new
int is_new_client(char *ip, int port){
    for(int i = 0; i < client_count; i++){
        if(strcmp(clients[i].ip, ip) == 0 )
        // && clients[i].port == port
            return 0;
    }
    strcpy(clients[client_count].ip, ip);
    clients[client_count].port = port;
    client_count++;
    return 1;
}

int main(){
    int server_sock, client_sock;
    struct sockaddr_in server, client;
    socklen_t client_size;
    char buffer[1024];

    // Fruit stock
    struct Fruit fruits[MAX_FRUITS] = {
        {"apple", 10},
        {"banana", 15},
        {"orange", 8},
        {"mango", 5},
        {"grape", 12}
    };

    // Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock < 0){
        perror("Socket creation failed.");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    if(bind(server_sock, (struct sockaddr*)&server, sizeof(server)) < 0){
        perror("Bind failed.");
        close(server_sock);
        return 1;
    }

    listen(server_sock, 5);
    printf("Fruit store server running on port %d...\n", PORT);

    while(1){
        client_size = sizeof(client);
        client_sock = accept(server_sock, (struct sockaddr*)&client, &client_size);
        if(client_sock < 0){
            perror("Accept failed");
            continue;
        }

        // Get client info
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client.sin_addr, ip, INET_ADDRSTRLEN);
        int port = ntohs(client.sin_port);

        printf("\nClient connected: %s:%d\n", ip, port);

        // Receive request
        int bytes = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
        if(bytes <= 0){
            close(client_sock);
            continue;
        }
        buffer[bytes] = '\0';
        printf("Client request: %s\n", buffer);

        // Parse message (fruit name + quantity)
        char fruit_name[30];
        int qty;
        sscanf(buffer, "%s %d", fruit_name, &qty);

        // Check fruit availability
        int found = 0;
        for (int i=0; i < MAX_FRUITS; i++){
            if(strcmp(fruits[i].name, fruit_name) == 0){
                found = 1;
                if(fruits[i].quantity >= qty){
                    fruits[i].quantity -= qty;
                    sprintf(buffer, "Sold %d %s(s). Remaining: %d\n", qty, fruit_name, fruits[i].quantity);
                }
                else{
                    sprintf(buffer, "Not enough %s in stock!\n", fruit_name);
                }
                send(client_sock, buffer, strlen(buffer), 0);
                break;
            }
        }

        if(!found){
            sprintf(buffer, "Fruit '%s' not found.\n", fruit_name);
            send(client_sock, buffer, strlen(buffer), 0);
        }

        // Track client
        is_new_client(ip, port);
        printf("Total unique customers: %d\n", client_count);

        // Send customer count
        sprintf(buffer, "Total unique customers: %d\n", client_count);
        send(client_sock, buffer, strlen(buffer), 0);

        close(client_sock);
    }

    close(server_sock);
    return 0;
}