#include <iostream>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int main() {
    int server_fd;
    int new_socket;

    struct sockaddr_in address;
    
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    
    // 1. Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 2. Attach socket to port 8080
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // listen on all interfaces
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 3. Listen for connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port 8080...\n";

    // 4. Accept a client
    new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if (new_socket < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // 5. Read message from client
    read(new_socket, buffer, 1024);
    std::cout << "Message from client: " << buffer << std::endl;

    // 6. Send response
    const char* hello = "Hello from server";
    send(new_socket, hello, strlen(hello), 0);

    close(new_socket);
    close(server_fd);
    return 0;
}
