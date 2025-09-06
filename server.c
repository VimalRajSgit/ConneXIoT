/*
* =====================================================================================
*
* Filename:  server.c
*
* Description:  A multi-threaded chat server using sockets.
*
* =====================================================================================
*/
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_PENDING_CONNS 10
#define MAX_CLIENTS 100

// Global state variables
int next_client_id = 1;
int session_counter = 1;
int user_pairings[MAX_CLIENTS]; // Stores the ID of the user each user is connected to
int client_sockets[MAX_CLIENTS]; // Stores the socket descriptor for each user
int session_log[MAX_CLIENTS]; // Stores session ID for a user's connection

// Structure to pass arguments to a new thread
typedef struct {
    int client_id;
    int client_fd;
    struct sockaddr_in client_address;
} thread_arg_t;

// Function Prototypes
void *client_handler_thread(void *arg);
void initialize_user_states();
void display_active_users();

// Initialize user state arrays
void initialize_user_states() {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        user_pairings[i] = -1; // -1 indicates not connected to anyone
        client_sockets[i] = -1;
    }
    user_pairings[0] = 0; // Sentinel value
}

int main(int argc, char *argv[]) {
    initialize_user_states();

    int port_num;
    int listen_fd, conn_fd;
    struct sockaddr_in server_addr;
    pthread_attr_t pthread_attr;
    thread_arg_t *thread_arg;
    pthread_t thread_id;

    // Get port from command line or prompt user
    if (argc > 1) {
        port_num = atoi(argv[1]);
    } else {
        printf("Please enter the server port number: ");
        scanf("%d", &port_num);
    }

    // Initialize server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_num);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Create the main listening socket
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the specified address and port
    if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Failed to bind socket");
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(listen_fd, MAX_PENDING_CONNS) == -1) {
        perror("Failed to listen on socket");
        exit(EXIT_FAILURE);
    }

    // Initialize thread attributes for creating detached threads
    pthread_attr_init(&pthread_attr);
    pthread_attr_setdetachstate(&pthread_attr, PTHREAD_CREATE_DETACHED);

    printf("Server is running and waiting for connections on port %d...\n", port_num);

    // Main loop to accept new connections
    while (1) {
        thread_arg = (thread_arg_t *)malloc(sizeof(thread_arg_t));
        if (!thread_arg) {
            perror("Failed to allocate memory for thread arguments");
            continue;
        }

        socklen_t client_addr_len = sizeof(thread_arg->client_address);
        conn_fd = accept(listen_fd, (struct sockaddr *)&thread_arg->client_address, &client_addr_len);
        if (conn_fd == -1) {
            perror("Failed to accept connection");
            free(thread_arg);
            continue;
        }

        // Setup arguments for the new client thread
        thread_arg->client_fd = conn_fd;
        thread_arg->client_id = next_client_id++;
        client_sockets[thread_arg->client_id] = conn_fd;


        // Create a new thread to handle the client
        if (pthread_create(&thread_id, &pthread_attr, client_handler_thread, (void *)thread_arg) != 0) {
            perror("Failed to create thread");
            free(thread_arg);
            continue;
        }
    }

    return 0;
}

// Print a list of currently connected user IDs
void display_active_users() {
    printf("Active users: [");
    for (int i = 1; i < MAX_CLIENTS; i++) {
        if (user_pairings[i] != -1) {
            printf("%d ", i);
        }
    }
    printf("]\n");
}

// Thread function to handle communication with a single client
void *client_handler_thread(void *arg) {
    char buffer[256];
    thread_arg_t *thread_data = (thread_arg_t *)arg;
    int client_socket = thread_data->client_fd;
    int client_id = thread_data->client_id;

    free(thread_data); // Free the argument structure, it's no longer needed

    user_pairings[client_id] = 0; // 0 indicates connected to server, but not in a chat session
    client_sockets[client_id] = client_socket;
    printf("User %d has connected.\n", client_id);
    display_active_users();

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);

        if (bytes_read <= 0) { // Client disconnected
            break;
        }

        if (strncmp(buffer, "~list", 5) == 0) {
            char list_response[256] = "Server: Available users [";
            for (int i = 1; i < MAX_CLIENTS; i++) {
                if (user_pairings[i] == 0) { // Find users who are free
                    char user_id_str[5];
                    sprintf(user_id_str, "%d,", i);
                    strcat(list_response, user_id_str);
                }
            }
            strcat(list_response, "]");
            write(client_socket, list_response, strlen(list_response));

        } else if (strncmp(buffer, "~connect_to_", 12) == 0) {
            int target_id = atoi(buffer + 12);
            char response_buffer[256];

            if (target_id < 1 || target_id >= MAX_CLIENTS || user_pairings[target_id] == -1) {
                strcpy(response_buffer, "Server: That user does not exist.");
            } else if (user_pairings[target_id] != 0) {
                strcpy(response_buffer, "Server: That user is currently busy.");
            } else if (target_id == client_id) {
                strcpy(response_buffer, "Server: You cannot connect to yourself.");
            } else {
                user_pairings[client_id] = target_id;
                user_pairings[target_id] = client_id;
                session_log[client_id] = session_counter;
                session_log[target_id] = session_counter;
                printf("Session %d created between user %d and user %d\n", session_counter++, client_id, target_id);
                sprintf(response_buffer, "Server: You are now connected to user %d", target_id);
            }
            write(client_socket, response_buffer, strlen(response_buffer));

        } else if (buffer[0] != '~') {
            int recipient_id = user_pairings[client_id];
            if (recipient_id > 0) {
                int recipient_sock = client_sockets[recipient_id];
                char message_to_send[280];
                sprintf(message_to_send, "%02d: %s", client_id, buffer);
                write(recipient_sock, message_to_send, strlen(message_to_send));
            } else {
                char *reply = "Server: You are not in a session. Use ~connect_to_<id> to start a chat.";
                write(client_socket, reply, strlen(reply));
            }

        } else if (strncmp(buffer, "~stop", 5) == 0) {
            int paired_user = user_pairings[client_id];
            if (paired_user == 0) {
                char *reply = "Server: You are not in any session.";
                write(client_socket, reply, strlen(reply));
            } else {
                user_pairings[client_id] = 0;
                user_pairings[paired_user] = 0;
                printf("Session %d between user %d and user %d has ended.\n", session_log[client_id], client_id, paired_user);
                char *reply = "Server: You have successfully logged out of the session.";
                write(client_socket, reply, strlen(reply));
            }
        } else if (strncmp(buffer, "~quit", 5) == 0) {
            char *reply = "Server: Disconnecting from server...";
            write(client_socket, reply, strlen(reply));
            break; // Exit the loop to disconnect
        } else if (strncmp(buffer, "~my_id", 6) == 0) {
            char id_response[32];
            sprintf(id_response, "Server: Your user ID is %02d", client_id);
            write(client_socket, id_response, strlen(id_response));
        }
    }

    // Cleanup when client disconnects
    if (user_pairings[client_id] > 0) {
        int paired_user = user_pairings[client_id];
        user_pairings[paired_user] = 0; // Mark the other user as free
        printf("Session %d between user %d and user %d has ended due to disconnect.\n", session_log[client_id], client_id, paired_user);
    }
    user_pairings[client_id] = -1; // Mark current user as fully disconnected
    client_sockets[client_id] = -1;
    printf("User %d has disconnected.\n", client_id);
    display_active_users();

    close(client_socket);
    return NULL;
}
