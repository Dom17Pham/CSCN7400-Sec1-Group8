#define _CRT_SECURE_NO_WARNINGS
#include "Common.h"
#include <WinSock2.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>

namespace common {
	std::string ReadFile(const std::string& filename) {
		std::string content = "";
		std::ifstream file(filename, std::ifstream::in);
		if (file.is_open()) {
			std::string line;
			while (std::getline(file, line)) {
				content += line;
			}
			file.close();
		}
		//If file did not open, meaning it has an error, it will return empty string.
		return content;
	}

	bool WriteFile(const std::string& filename, const std::string& content) {
		bool isWritten = false;
		std::ofstream file(filename, std::ofstream::out);
		if (file.is_open()) {
			file << content;
			file.close();
			isWritten = true;
		}
		return isWritten;
	}

	uint64_t generateUniqueID() {
		srand(static_cast<unsigned>(time(nullptr)));
		uint64_t id = rand() % 1000 * 1000 + rand() % 1000;
		return id;
	}

	uint32_t calculateChecksum(const DataPacket& packet) {

		const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&packet);
		uint32_t checksum = 0;
		for (size_t i = 0; i < sizeof(DataPacket); ++i) {
			checksum += bytes[i];
		}
		return checksum;
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

	void getTimeStamp(const DataPacket& packet) {
		std::chrono::system_clock::time_point time_point = std::chrono::system_clock::time_point(std::chrono::milliseconds(packet.header.timestamp));
		std::time_t time = std::chrono::system_clock::to_time_t(time_point);
		std::ostringstream oss;
		oss << std::put_time(gmtime(&time), "%Y-%m-%d %H:%M:%S UTC");
		std::cout << "Timestamp: " << oss.str() << std::endl;
	}
}