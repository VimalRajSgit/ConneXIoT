/*
* =====================================================================================
*
* Filename:  client.c
*
* Description:  A simple chat client to connect to the server.
*
* =====================================================================================
*/
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define HOSTNAME_LEN_MAX 255

int main(int argc, char *argv[]) {
    char hostname[HOSTNAME_LEN_MAX + 1] = {0};
    int port, sock_fd;
    struct hostent *server;
    struct sockaddr_in server_addr;

    // Get server hostname from command line arguments or stdin.
    if (argc > 1) {
        strncpy(hostname, argv[1], HOSTNAME_LEN_MAX);
    } else {
        printf("Enter Server Hostname: ");
        scanf("%s", hostname);
    }

    // Get server port from command line arguments or stdin.
    port = (argc > 2) ? atoi(argv[2]) : 0;
    if (!port) {
        printf("Enter Server Port: ");
        scanf("%d", &port);
    }

    // Resolve server hostname to an IP address.
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    // Initialize server address structure.
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    // Create a TCP socket.
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Failed to open socket");
        exit(1);
    }

    // Connect to the server.
    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Failed to connect to server");
        exit(1);
    }
    
    printf("Connected to the server. Type your message or command.\n");

    // Main communication loop
    while (1) {
        char buffer[256];
        printf("> ");
        memset(buffer, 0, sizeof(buffer));
        
        // Use fgets to read a whole line, including spaces
        // We need to clear stdin from the previous scanf before the loop
        if (ftell(stdin) == 0) { // A simple way to check if it's the first input
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
        fgets(buffer, sizeof(buffer) - 1, stdin);
        buffer[strcspn(buffer, "\n")] = 0; // Remove trailing newline from fgets


        // Write the message to the socket
        ssize_t io_result = write(sock_fd, buffer, strlen(buffer));
        if (io_result < 0) {
            perror("ERROR writing to socket");
            break;
        }

        // Read the server's reply
        memset(buffer, 0, sizeof(buffer));
        io_result = read(sock_fd, buffer, sizeof(buffer) - 1);
        if (io_result < 0) {
            perror("ERROR reading from socket");
            break;
        }
        
        if (io_result == 0) {
            printf("\nServer closed the connection.\n");
            break;
        }

        // Display the received message
        if (strncmp(buffer, "Server:", 7) == 0) {
            printf("%s\n", buffer);
        } else {
            printf("Message from User %s\n", buffer);
        }
        
        // Break the loop if the server disconnect message is received
        if (strstr(buffer, "Disconnecting from server") != NULL) {
            break;
        }
    }

    // Close the socket and exit.
    close(sock_fd);
    return 0;
}
