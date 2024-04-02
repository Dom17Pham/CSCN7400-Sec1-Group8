#pragma once

namespace common {
	struct Location {
		double x;
		double y;
	};

	struct Boundary {
		double xMin;
		double xMax;
		double yMin;
		double yMax;
	};

	// GC boundary is a rectangle
	const Location groundControlLocation[] = {
		{0.0 , 0.0},
		{70.0 , 70.0},
		{-25.0 , -80.0},
	};

	// How far it extends from the center of the rectangle
	const common::Location extends = { 100.0 , 50.0 };

	// The origin (0,0) of the Map is left bottom
	class Map {
	public:
		static double GetDistanceBetweenAB(Location A, Location B);
		static Boundary GetBoundaryFromLocationAndExtends(Location location, Location extends);
		static bool isLocationInsideBoundary(Location location, Boundary boundary);
		static bool isBoundaryOverlap(Boundary A, Boundary B);
	};
}