#include <iostream>
#include <string>
#include <thread>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")
const int PORT = 8080;

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
        std::cerr << "Socket creation error" << std::endl;
        WSACleanup();
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket 
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return -1;
    }

    // Listen for connections
    if (listen(server_fd, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed" << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return -1;
    }

    std::cout << "Ground Control is listening on port " << PORT << std::endl;

    // Accept incoming connection
    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) == INVALID_SOCKET) {
        std::cerr << "Accept failed" << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return -1;
    }

    while (true) {
        
        char buffer[1024] = { 0 };
        int bytesReceived = recv(new_socket, buffer, sizeof(buffer), 0);

        // Client is disconnected when server receives no data from client
        if (bytesReceived <= 0) {
            std::cerr << "Client disconnected" << std::endl;
            break;
        }

        std::cout << "Received timestamp from client: " << buffer << std::endl;

        // Send confirmation of receiving data from client
        const char* response = "Received";
        send(new_socket, response, strlen(response), 0);

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    closesocket(new_socket);
    closesocket(server_fd);
    WSACleanup();
    return 0;
}
