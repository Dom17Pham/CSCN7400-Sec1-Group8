#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <WinSock2.h>
#include "DataPacket.h"

#include "Common.h"

#pragma comment(lib, "ws2_32.lib")
const int PORT = 8080;

uint32_t calculateChecksum(const DataPacket& packet) {

    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&packet);
    uint32_t checksum = 0;
    for (size_t i = 0; i < sizeof(DataPacket); ++i) {
        checksum += bytes[i];
    }
    return checksum;
}

uint64_t generateUniqueID() {
    srand(static_cast<unsigned>(time(nullptr)));
    uint64_t id = rand() % 1000 * 1000 + rand() % 1000;
    return id;
}

void sendPacket(SOCKET sock, const DataPacket& packet) {

    uint32_t checksum = calculateChecksum(packet);

    DataPacket packetToSend = packet;

    packetToSend.CRCchecksum = checksum;

    send(sock, reinterpret_cast<const char*>(&packetToSend), sizeof(DataPacket), 0);
}

void receivePacket(SOCKET sock, DataPacket& packet) {

    recv(sock, reinterpret_cast<char*>(&packet), sizeof(DataPacket), 0);
    uint32_t calculatedChecksum = calculateChecksum(packet);
    //if (packet.CRCchecksum != calculatedChecksum) {
    //    std::cerr << "Checksum validation failed" << std::endl;
    //}
}

std::string serializePacket(const DataPacket& packet) {
    std::ostringstream oss;
    oss.write(reinterpret_cast<const char*>(&packet), sizeof(DataPacket));
    return oss.str();
}

DataPacket deserializePacket(const char* buffer) {
    DataPacket packet;
    memcpy(&packet, buffer, sizeof(DataPacket));
    return packet;
}

void DisplayData(const DataPacket& packet) {

    std::cout << std::endl;
    std::chrono::system_clock::time_point time_point = std::chrono::system_clock::time_point(std::chrono::milliseconds(packet.header.timestamp));
    std::time_t time = std::chrono::system_clock::to_time_t(time_point);
    std::ostringstream oss;
    oss << std::put_time(gmtime(&time), "%Y-%m-%d %H:%M:%S UTC");
    std::cout << "Timestamp: " << oss.str() << std::endl;
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

    uint64_t controlID = generateUniqueID();
    std::cout << "Ground Control is listening on port " << PORT << std::endl;
    
    while (true) {

        // Accept incoming connection
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) == INVALID_SOCKET) {
            std::cerr << "Accept failed" << std::endl;
            closesocket(server_fd);
            WSACleanup();
            return -1;
        }

        bool clientConnected = true;

        while (clientConnected) {
            DataPacket Recvpacket;
            DataPacket Sendpacket;

            auto now = std::chrono::system_clock::now();
            auto milliseconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
            uint64_t timestamp = milliseconds_since_epoch;

            // Receive packet from client
            int bytesReceived = recv(new_socket, reinterpret_cast<char*>(&Recvpacket), sizeof(DataPacket), 0);
            if (bytesReceived <= 0) {
                std::cerr << "Client disconnected" << std::endl;
                clientConnected = false; 
                break; 
            }
            Recvpacket = deserializePacket(reinterpret_cast<const char*>(&Recvpacket));
            DisplayData(Recvpacket);

            Sendpacket.header.packetType = 0;
            Sendpacket.header.timestamp = timestamp;
            Sendpacket.header.sequenceNumber = Recvpacket.header.sequenceNumber + 1;
            Sendpacket.payload0.controlID = controlID;
            Sendpacket.payload0.controlCommands = 0;
            Sendpacket.payload0.systemStatus = 1;
            std::string serializedPacket = serializePacket(Sendpacket);
            send(new_socket, serializedPacket.c_str(), serializedPacket.size(), 0);

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        closesocket(new_socket); 
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}
