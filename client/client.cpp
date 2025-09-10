#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

// the port number the server is listening on
#define PORT 8080

/*
    A simple TCP client that connects to a server (hostname given as argument),
    sends a message, and waits for a response.
*/

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <server_host>\n";
        return 1;
    }

    const char* server_host = argv[1];
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // TCP socket creation
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation error");
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Resolve hostname to IP
    struct hostent* server = gethostbyname(server_host);
    if (server == NULL) {
        std::cerr << "Error: no such host " << server_host << "\n";
        return 1;
    }
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    // Connect to server
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }

    std::cout << "Connected to server (" << server_host << ")!\n";
    
    while (true) {
        std::string msg;
        std::cout << "Client: ";
        std::getline(std::cin, msg);

        send(sock, msg.c_str(), msg.size(), 0);

        if (msg == "exit") break;

        memset(buffer, 0, sizeof(buffer));
        int valread = read(sock, buffer, 1024);
        if (valread <= 0) break;

        std::cout << "Server: " << buffer << std::endl;
    }

    close(sock);
    return 0;
}
