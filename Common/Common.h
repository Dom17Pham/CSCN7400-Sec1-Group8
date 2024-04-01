#pragma once
#include <cstdint>
#include <string>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

namespace common {
	struct Location {
		double x;
		double y;
	};

	std::string ReadFile(const std::string& filename);
	bool WriteFile(const std::string& filename, const std::string& content);

    struct Header {
        uint8_t packetType;
        uint32_t packetSize;
        uint32_t sequenceNumber;
        uint64_t timestamp;
    };

    struct AircraftPayload {
        uint64_t aircraftID;
        double latitude;
        double longitude;
        double altitude;
        double speed;
        double heading;
        int flightStatus;
    };

    struct GCPayload {
        uint64_t controlID;
        int controlCommands;
        int systemStatus;
    };

    struct Footer {
        uint32_t errorCode;
    };

    struct DataPacket {
        Header header;
        AircraftPayload payload1;
        GCPayload payload0;
        uint32_t CRCchecksum;
        Footer footer;
    };

    uint64_t generateUniqueID();
    void getTimeStamp(const DataPacket& packet);
    uint32_t calculateChecksum(const DataPacket& packet);
    void sendPacket(SOCKET sock, const DataPacket& packet);
    void receivePacket(SOCKET sock, DataPacket& packet);
    std::string serializePacket(const DataPacket& packet);
    DataPacket deserializePacket(const char* buffer);

}