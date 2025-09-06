Dockerized C Chat Application
This project contains a simple multi-threaded chat server and a corresponding client, both written in C. The applications are containerized using Docker for easy deployment and scalability, making them suitable for cloud environments like Azure.

Files
server.c: The source code for the multi-threaded chat server.

client.c: The source code for the interactive chat client.

Makefile: Used to compile both the server and client executables.

Dockerfile.server: Docker instructions to build the server image.

Dockerfile.client: Docker instructions to build the client image.

README.md: This instruction file.

How to Run
Prerequisites
Docker must be installed on your system.

Step 1: Build the Docker Images
First, build the Docker images for both the server and the client using the provided Dockerfiles.

Open your terminal in the project directory and run the following commands:

# Build the server image and tag it as 'chat-server'
docker build -t chat-server -f Dockerfile.server .

# Build the client image and tag it as 'chat-client'
docker build -t chat-client -f Dockerfile.client .

Step 2: Run the Server Container
Next, run the server container. We will map port 8000 on your local machine to the exposed port 8000 inside the container.

# Run the server in detached mode (-d) and map the port (-p)
docker run -d -p 8000:8000 --name chat-server-container chat-server

You can check if the server is running by looking at its logs:

docker logs chat-server-container

Step 3: Run the Client Container(s)
Now you can run one or more client containers to connect to the server. We run the client in interactive mode (-it) so you can type messages.

We use --network="host" to make networking simpler for local testing, allowing the client container to connect to localhost on your machine where the server port is mapped.

# Run the client in interactive mode
docker run -it --rm --network="host" chat-client

Once inside the client container's shell, start the client application:

./chat_client localhost 8000

You can open multiple terminal windows and run the docker run command for the client in each to simulate multiple users.

Azure & IoT Connectivity
This Docker setup is the first step for cloud deployment. To deploy on Azure, you would:

Push your chat-server image to a container registry like Azure Container Registry (ACR).

Deploy the image to an Azure service like Azure Container Instances (ACI) or Azure Kubernetes Service (AKS).

Ensure the network security groups in Azure are configured to allow traffic on port 8000.

Your IoT devices or other clients would then connect to the public IP address of your deployed Azure service."# ConneXIoT" 
"# ConneXIoT" 
