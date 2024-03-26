#pragma once
#include <cstdint>
#include <string>

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