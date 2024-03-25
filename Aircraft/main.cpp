#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

const char* SERVER_IP = "127.0.0.1"; // Server IP address
const int PORT = 8080;

int main() {

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET sock = INVALID_SOCKET;
    struct sockaddr_in server;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
        std::cerr << "Socket creation error" << std::endl;
        WSACleanup();
        return -1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to server
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        std::cerr << "Connection Failed" << std::endl;
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    while (true) {

        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);

        std::ostringstream oss;
        oss << std::put_time(gmtime(&now_c), "%Y-%m-%d %H:%M:%S UTC");
        std::string timestamp = oss.str();

        // Send timestamp to server
        send(sock, timestamp.c_str(), timestamp.size(), 0);

        // Receive response from server
        char response[1024] = { 0 };
        recv(sock, response, sizeof(response), 0);
        std::cout << response << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));

    }

    closesocket(sock);
    WSACleanup();
    return 0;
}