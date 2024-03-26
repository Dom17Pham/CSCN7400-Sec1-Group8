#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <WinSock2.h>
#include <cstdlib>
#include <ctime>
#include "DataPacket.h"

#pragma comment(lib, "ws2_32.lib")

const char* SERVER_IP = "127.0.0.1"; // Server IP address
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
    //uint32_t calculatedChecksum = calculateChecksum(packet);
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
    std::cout << "Ground Control ID: " << packet.payload0.controlID << std::endl;
    std::cout << "Control Commands: " << packet.payload0.controlCommands << std::endl;
    std::cout << "System Status: " << packet.payload0.systemStatus << std::endl;
    
}


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

    uint64_t aircraftID = generateUniqueID();
    uint32_t sequenceNum = 0;
    u_long mode = 1;
    ioctlsocket(sock, FIONBIO, &mode);

    while (true) {

        auto now = std::chrono::system_clock::now();
        auto milliseconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        uint64_t timestamp = milliseconds_since_epoch;

        DataPacket Recvpacket;
        DataPacket Sendpacket;
        Sendpacket.header.timestamp = timestamp;
        Sendpacket.payload1.aircraftID = aircraftID;
        Sendpacket.header.packetType = 1;
        Sendpacket.header.sequenceNumber = sequenceNum;
        sequenceNum++;
        std::string serializedPacket = serializePacket(Sendpacket);
        send(sock, serializedPacket.c_str(), serializedPacket.size(), 0);

        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(sock, &readSet);
        timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        int selectResult = select(sock + 1, &readSet, nullptr, nullptr, &timeout);
        if (selectResult > 0 && FD_ISSET(sock, &readSet)) {
            DataPacket Recvpacket;
            receivePacket(sock, Recvpacket);
            DisplayData(Recvpacket);
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));

    }

    closesocket(sock);
    WSACleanup();
    return 0;
}