#include "Map.h"
#include <cmath>

namespace common {
	double Map::GetDistanceBetweenAB(Location A, Location B)
	{
		// Distance formula
		return sqrt(pow(B.x - A.x, 2) + pow(B.y - A.y, 2));
	}

	Boundary Map::GetBoundaryFromLocationAndExtends(Location location, Location extends)
	{
		// Calculates rect bound
		double xMin = location.x - extends.x;
		double xMax = location.x + extends.x;
		double yMin = location.y - extends.y;
		double yMax = location.y + extends.y;
		return Boundary{ xMin, xMax, yMin, yMax };
	}

	bool Map::isLocationInsideBoundary(Location location, Boundary boundary)
	{
		// Check if a point is inside a rect bound
		return (location.x >= boundary.xMin && location.x <= boundary.xMax &&
			location.y >= boundary.yMin && location.y <= boundary.yMax);
	}

	bool Map::isBoundaryOverlap(Boundary A, Boundary B)
	{
		// Check if two rect bound overlaps
		return (A.xMin <= B.xMax && A.xMax >= B.xMin && A.yMin <= B.yMax && A.yMax >= B.yMin);
	}
}

