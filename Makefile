# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -pthread

# Target executables
TARGETS = chat_server chat_client

# Default target
all: $(TARGETS)

# Rule to build the server
chat_server: server.c
	$(CC) $(CFLAGS) -o chat_server server.c $(LDFLAGS)

# Rule to build the client
chat_client: client.c
	$(CC) $(CFLAGS) -o chat_client client.c

# Clean up compiled files
clean:
	rm -f $(TARGETS)
