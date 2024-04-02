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
		double minX = location.x - extends.x;
		double right = location.x + extends.x;
		double top = location.y + extends.y;
		double bottom = location.y - extends.y;
		return Boundary{ minX, right, top, bottom };
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

