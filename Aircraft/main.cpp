#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include "Common.h"
#include "Aircraft.h"
using namespace common;

const char* SERVER_IP = "127.0.0.1"; // Server IP address
const int BASE_PORT = 8080;

void DisplayData(const DataPacket& packet) {

	std::cout << std::endl;
	getTimeStamp(packet);
	std::cout << "Sequence Number: " << packet.header.sequenceNumber << std::endl;
	std::cout << "Ground Control ID: " << packet.payload0.controlID << std::endl;
	std::cout << "Control Commands: " << packet.payload0.controlCommands << std::endl;
	std::cout << "System Status: " << packet.payload0.systemStatus << std::endl;

}


int main(int argc, char* argv[]) {
	// Program Startup
	if (argc > 2) {
		std::cerr << "Please only enter one program argument with an integer of range 1 - 10.\n";
		return -1;
	}

	int aircraftID = std::stoi(argv[1]);

	if (aircraftID < 1 || aircraftID > 10) {
		std::cerr << "Please only enter one program argument with an integer of range 1 - 10.\n";
		return -1;
	}

	aircraft::Aircraft aircraft(aircraftID);



	while (aircraft.getAircraftStatus() != aircraft::AircraftStatus::STOP) {
		int GCID = aircraft.getGroupControlConnection();
		int port = BASE_PORT + GCID;

		// Network Initialization
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
		server.sin_port = htons(port);
		server.sin_addr.s_addr = inet_addr(SERVER_IP);

		// Connect to server
		if (connect(sock, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
			std::cerr << "Connection Failed" << std::endl;
			closesocket(sock);
			WSACleanup();
			return -1;
		}

		//uint64_t aircraftID = generateUniqueID();
		uint32_t sequenceNum = 0;
		u_long mode = 1;
		ioctlsocket(sock, FIONBIO, &mode);

		// Simulation Loop
		while (GCID == aircraft.getGroupControlConnection()) {
			aircraft.updateAircraftUI();
			auto now = std::chrono::system_clock::now();
			auto milliseconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
			uint64_t timestamp = milliseconds_since_epoch;

			DataPacket Sendpacket;
			DataPacket Recvpacket;
			AircraftPayload aircraftPayload = {
				(uint64_t)aircraftID,
				aircraft.getCurrentLocation().x,
				aircraft.getCurrentLocation().y,
				10000.0, 99999.99999, 0.0, 1 };

			Sendpacket.header.timestamp = timestamp;
			Sendpacket.payload1 = aircraftPayload;
			Sendpacket.header.packetType = 1;
			Sendpacket.header.sequenceNumber = sequenceNum;
			sequenceNum++;
			sendPacket(sock, Sendpacket);

			fd_set readSet;
			FD_ZERO(&readSet);
			FD_SET(sock, &readSet);
			timeval timeout;
			timeout.tv_sec = 0;
			timeout.tv_usec = 0;
			int selectResult = select(sock + 1, &readSet, nullptr, nullptr, &timeout);
			if (selectResult > 0 && FD_ISSET(sock, &readSet)) {
				Recvpacket = receivePacket(sock);
				DisplayData(Recvpacket);
			}

			std::this_thread::sleep_for(std::chrono::seconds(1));
			aircraft.updateAircraftLocation();
		}

		closesocket(sock);
		WSACleanup();
	}

	return 0;
}