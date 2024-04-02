#pragma once

#include "Map.h"
#include "string"

namespace aircraft {
    typedef char TailNumber[7];
    const TailNumber tailNumberList[] = {
        "N974TX",
        "N816PY",
        "N845AK",
        "N154UR",
        "N760NT",
        "N342AI",
        "N127AS",
        "N248GW",
        "N997JM",
        "N609FA"
    };

    const common::Location origin = { -120.0, -120.0 };
    const common::Location destination = { 160, 110 };

    enum AircraftStatus
    {
        START, STOP
    };   

    struct FlightPlan {
        common::Location origin;
        common::Location destination;
        double totalDistance;
    };

    class Aircraft {
    public:
        Aircraft();
        Aircraft(int id);
        ~Aircraft();
        void updateAircraftUI();
        void updateAircraftLocation();
        int getClosestGroundControl();
        AircraftStatus getAircraftStatus();
        common::Location getCurrentLocation();
        int getGroupControlConnection();
    private:
        AircraftStatus aircraftStatus;
        double flightCompletionPercentage;
        std::string tailNumber;
        common::Location currentLocation;
        FlightPlan flightPlan;
        /*
        Aircraft only connected to one ground control at a time,
        for ToC, when the aircraft detects itself entering another GC boundary,
        it will disconnect the current GC and establish connect with the new GC.
        */
        int groupControlConnection;
    };
}