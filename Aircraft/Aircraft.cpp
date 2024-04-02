#include "Aircraft.h"
#include "Map.h"
#include <iostream>
#include <chrono>
#include <limits>

namespace aircraft {
	Aircraft::Aircraft() : aircraftStatus{ AircraftStatus::START }, flightCompletionPercentage{ 0.0 }, tailNumber{ "" },
		currentLocation{ 0.0, 0.0 }, flightPlan{ {0.0, 0.0}, {0.0, 0.0}, 0.0 }, groupControlConnection{ -1 } {}

	Aircraft::Aircraft(int id) : Aircraft()
	{
		tailNumber = tailNumberList[id];
		currentLocation = origin;
		flightPlan.origin = origin;
		flightPlan.destination = destination;
		flightPlan.totalDistance = common::Map::GetDistanceBetweenAB(currentLocation, destination);
		groupControlConnection = getClosestGroundControl();
	}

	Aircraft::~Aircraft() {}

	void Aircraft::updateAircraftUI()
	{
		std::cout << "\x1B[2J\x1B[H"; //Clear Console Screen

		std::cout << "Aircraft Tail Number: " << tailNumber << std::endl;
		std::cout << "Aircraft Status:\n";
		std::cout << "----------------\n";
		std::cout << "Not Latitude: " << currentLocation.x << "\n";
		std::cout << "Not Longitude: " << currentLocation.y << "\n";
		std::cout << "Flight Trip Completion Percentage: " << flightCompletionPercentage * 100 << "%\n";
	}

	// Update Aircraft Location based on percentage of the flight completed. 1 Second = 1 Percent.
	void Aircraft::updateAircraftLocation()
	{
		groupControlConnection = getClosestGroundControl();

		if (flightCompletionPercentage > 1.0) {
			aircraftStatus = AircraftStatus::STOP;
			groupControlConnection = -1;
		}

		double deltaX = flightPlan.destination.x - flightPlan.origin.x;
		double deltaY = flightPlan.destination.y - flightPlan.origin.y;

		currentLocation.x = flightPlan.origin.x + deltaX * flightCompletionPercentage;
		currentLocation.y = flightPlan.origin.y + deltaY * flightCompletionPercentage;

		flightCompletionPercentage += 0.01;
	}

	int Aircraft::getClosestGroundControl()
	{
		int numOfGroundControl = sizeof(common::groundControlLocation) / sizeof(common::Location);
		int closestGCID = -1;	
		double closestDistance = std::numeric_limits<double>::max();
		for (int i = 0; i < numOfGroundControl; i++) {
			double distance = common::Map::GetDistanceBetweenAB(currentLocation, common::groundControlLocation[i]);
			if (distance < closestDistance) {
				closestDistance = distance;
				closestGCID = i;
			}
		}
		return closestGCID;
	}

	AircraftStatus Aircraft::getAircraftStatus()
	{
		return aircraftStatus;
	}

	common::Location Aircraft::getCurrentLocation()
	{
		return currentLocation;
	}
	int Aircraft::getGroupControlConnection()
	{
		return groupControlConnection;
	}
}