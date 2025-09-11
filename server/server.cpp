#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>

//Macro to set up the port number for the server to listen on
#define PORT 8080

/*
    A simple TCP server that listens on port 8080, accepts a connection, reads a message, and sends a response.
    Waits for a client to connect and interact.
*/

int main() {
    int server_fd, new_socket; // file descriptors for the server and the accepted socket (client)
    struct sockaddr_in address; // struct to hold the server address information
    int opt = 1; // option for setsockopt 
    int addrlen = sizeof(address); // length of the address struct
    char buffer[1024] = {0}; // buffer to hold incoming messages from client 

    /* 
        Create a TCP socket
        AF_INET: IPv4
        SOCK_STREAM: TCP
        0: default protocol (TCP for SOCK_STREAM)
    */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    //Allows the socket to be reused immediately after the program terminates
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    //Fills the struct with the server address info
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    /*
        Binds the socket to the address (IP and port)
        -- Commands OS to direct traffic on PORT to this socket --

        'listen' marks the socket as passive and ready to accept incoming connection requests
        -- 3 is the maximum number of pending connections in the queue (Backlog)--
    */
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    std::cout << "Server listening on port " << PORT << "...\n";

    // Waits for a client to connect. Blocking call until someone connects, returns a socket.
    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    
    //std::cout << "Client connected!\n";
    //std::cout << "Server Listening!\n";
    
    // Main loop 
    while (true) {
        //wait for a client to connect
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);

        //Spawn worker thread to handle client communication
        std::thread clientThread(HandleClient, new_socket);
        // Detach the thread to allow independent execution
        clientThread.detach(); 
        //continue to accept new clients

    }

    close(new_socket); // close the client socket
    close(server_fd); // close the server socket
    return 0;
}

// Worker thread Function to handle client communication in a separate thread//
    void HandleClient(int socket)
    {
        //space for client message and server reply
        char buffer[1024] = {0};
        while (true) {

            //read messages from socket
            memset(buffer, 0, sizeof(buffer));
            int valread = read(socket, buffer, 1024);
            if (valread <= 0) break;

            /*
                output client message
                convert char array to string for easier handling
            */
            std::string msg(buffer);
            std::cout << "Client: " << msg;

            if (msg == "exit" || msg == "exit\n") {
                std::cout << "Client disconnected.\n";
                break;
            }

            //get server reply and send back to client
            std::string reply;
            std::cout << "Server: ";
            std::getline(std::cin, reply);

            send(socket, reply.c_str(), reply.size(), 0);

            if (reply == "exit") break;
        }
        close(socket);
    }
