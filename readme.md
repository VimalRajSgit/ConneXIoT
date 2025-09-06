# ConneXIoT — Dockerized C Chat Application

A simple multi-threaded **chat server** and **chat client** written in C, containerized with **Docker** for easy deployment on local machines or cloud platforms like **Azure**.

---

## 📂 Files
- `server.c` — Multi-threaded chat server  
- `client.c` — Interactive chat client  
- `Makefile` — Compile server & client  
- `Dockerfile.server` — Build server image  
- `Dockerfile.client` — Build client image  
- `README.md` — Project documentation  

---

## 🛠️ Build with Makefile
```bash
make        # build executables
make clean  # remove executables
```

---

## 📦 Docker Setup

### Server
```dockerfile
FROM gcc:11 AS builder
WORKDIR /app
COPY . .
RUN make chat_server

FROM debian:bullseye-slim
WORKDIR /app
COPY --from=builder /app/chat_server .
EXPOSE 8000
CMD ["./chat_server", "8000"]
```

### Client
```dockerfile
FROM gcc:11 AS builder
WORKDIR /app
COPY . .
RUN make chat_client

FROM debian:bullseye-slim
WORKDIR /app
COPY --from=builder /app/chat_client .
CMD ["/bin/bash"]
```

---

## 🚀 Run

```bash
# Build images
docker build -t chat-server -f Dockerfile.server .
docker build -t chat-client -f Dockerfile.client .

# Run server
docker run -d -p 8000:8000 --name chat-server-container chat-server

# Run client
docker run -it --rm --network="host" chat-client
./chat_client localhost 8000
```

---

## ☁️ Azure Deployment
1. Push server image to **Azure Container Registry**  
2. Deploy via **Azure Container Instances (ACI)** or **AKS**  
3. Open port **8000** for clients/IoT devices  

---

## 🔖 Project
**ConneXIoT**
