# ConneXIoT — Dockerized C Chat Application

This project contains a simple **multi-threaded chat server** and a corresponding **chat client**, both written in C.  
The applications are containerized using **Docker** for easy deployment and scalability, making them suitable for cloud environments like **Azure** or IoT use cases.

---

## 📂 Project Structure

- **server.c** → Source code for the multi-threaded chat server  
- **client.c** → Source code for the interactive chat client  
- **Makefile** → Used to compile both the server and client executables  
- **Dockerfile.server** → Docker instructions to build the server image  
- **Dockerfile.client** → Docker instructions to build the client image  
- **README.md** → This documentation  

---

## 🛠️ Makefile

```make
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
Usage:

bash
Copy code
make        # builds chat_server and chat_client
make clean  # removes executables
📦 Dockerfiles
🔹 Dockerfile.server
dockerfile
Copy code
FROM gcc:11 AS builder

WORKDIR /app
COPY . .

RUN make chat_server

FROM debian:bullseye-slim
WORKDIR /app
COPY --from=builder /app/chat_server .

EXPOSE 8000
CMD ["./chat_server", "8000"]
🔹 Dockerfile.client
dockerfile
Copy code
FROM gcc:11 AS builder

WORKDIR /app
COPY . .

RUN make chat_client

FROM debian:bullseye-slim
WORKDIR /app
COPY --from=builder /app/chat_client .

CMD ["/bin/bash"]
🚀 How to Run
Prerequisites
Docker installed on your system

Step 1: Build the Docker Images
bash
Copy code
# Build the server image and tag it as 'chat-server'
docker build -t chat-server -f Dockerfile.server .

# Build the client image and tag it as 'chat-client'
docker build -t chat-client -f Dockerfile.client .
Step 2: Run the Server Container
bash
Copy code
# Run the server in detached mode (-d) and map port 8000
docker run -d -p 8000:8000 --name chat-server-container chat-server
Check server logs:

bash
Copy code
docker logs chat-server-container
Step 3: Run the Client Container(s)
bash
Copy code
# Run the client in interactive mode
docker run -it --rm --network="host" chat-client
Once inside the container shell, start the client:

bash
Copy code
./chat_client localhost 8000
💡 Open multiple terminals and run the above command to simulate multiple users.

☁️ Azure & IoT Deployment
This Docker setup is the foundation for cloud deployment. On Azure:

Push your chat-server image to Azure Container Registry (ACR).

Deploy it to Azure Container Instances (ACI) or Azure Kubernetes Service (AKS).

Configure network security groups to allow traffic on port 8000.

IoT devices or other clients can then connect to the public IP address of your deployed service.

🔖 Project Name
ConneXIoT

pgsql
Copy code

Would you like me to also add a **diagram (architecture flow)** in the README (like server ↔ client ↔ Azure cloud) so it looks even more professional for your project?






