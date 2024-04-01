#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <sstream>
#include <iomanip>
#include "Common.h"
#include <vector>
#include <mutex>

using namespace common;

const int PORT = 8080;

uint64_t controlID = generateUniqueID();

void DisplayData(const DataPacket& packet) {

    std::cout << std::endl;
    getTimeStamp(packet);
    std::cout << "Sequence Number: " << packet.header.sequenceNumber << std::endl;

    if (packet.header.packetType == 1) { 
        std::cout << "Aircraft ID: " << packet.payload1.aircraftID << std::endl;
        std::cout << "Latitude: " << packet.payload1.latitude << std::endl;
        std::cout << "Longitude: " << packet.payload1.longitude << std::endl;
        std::cout << "Altitude: " << packet.payload1.altitude << std::endl;
        std::cout << "Speed: " << packet.payload1.speed << std::endl;
        std::cout << "Heading: " << packet.payload1.heading << std::endl;
        std::cout << "Flight Status: " << packet.payload1.flightStatus << std::endl;
    }
    else if (packet.header.packetType == 0) { 
        std::cout << "Ground Control ID: " << packet.payload0.controlID << std::endl;
        std::cout << "Control Commands: " << packet.payload0.controlCommands << std::endl;
        std::cout << "System Status: " << packet.payload0.systemStatus << std::endl;
    }
}

void ClientHandler(SOCKET client_socket) {

    bool clientConnected = true;

    while (clientConnected) {
        DataPacket Recvpacket;
        DataPacket Sendpacket;

        auto now = std::chrono::system_clock::now();
        auto milliseconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        uint64_t timestamp = milliseconds_since_epoch;

        Recvpacket = receivePacket(client_socket);

        DisplayData(Recvpacket);

        Sendpacket.header.packetType = 0;
        Sendpacket.header.timestamp = timestamp;
        Sendpacket.header.sequenceNumber = Recvpacket.header.sequenceNumber + 1;
        Sendpacket.payload0.controlID = controlID;
        Sendpacket.payload0.controlCommands = 0;
        Sendpacket.payload0.systemStatus = 1;
        sendPacket(client_socket, Sendpacket);

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    closesocket(client_socket);
}

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
    
    while (true) {

        // Accept incoming connection
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) == INVALID_SOCKET) {
            std::cerr << "Accept failed" << std::endl;
            closesocket(server_fd);
            WSACleanup();
            return -1;
        }

        std::thread clientThread(ClientHandler, new_socket);
        clientThread.detach();
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}
