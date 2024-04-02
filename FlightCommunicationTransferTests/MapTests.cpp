#include "CppUnitTest.h"

#include "Map.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace common;

namespace FlightCommunicationTransferTests
{
	TEST_CLASS(MapTests)
	{
	public:

		TEST_METHOD(MAP_001_GetDistanceBetweenABTest)
		{
			// Arrange
			Location A{ 0, 0 };	// Triangle with side 3, 4, 5
			Location B{ 3, 4 };
			double expectedDistance = 5.0;

			// Act
			double actualDistance = Map::GetDistanceBetweenAB(A, B);

			// Assert
			Assert::AreEqual(expectedDistance, actualDistance, 0.0001);
		}

		TEST_METHOD(MAP_002_GetBoundaryFromLocationAndExtendsTest)
		{
			// Arrange
			Location location{ 10, 20 };
			Location extends{ 5, 7 };
			Boundary expectedBoundary{ 5, 15, 13, 27 };

			// Act
			Boundary actualBoundary = Map::GetBoundaryFromLocationAndExtends(location, extends);

			// Assert
			Assert::AreEqual(expectedBoundary.xMin, actualBoundary.xMin, 0.0001);
			Assert::AreEqual(expectedBoundary.xMax, actualBoundary.xMax, 0.0001);
			Assert::AreEqual(expectedBoundary.yMin, actualBoundary.yMin, 0.0001);
			Assert::AreEqual(expectedBoundary.yMax, actualBoundary.yMax, 0.0001);
		}

		TEST_METHOD(MAP_003_IsLocationInsideBoundaryTest_PointIsInsideBoundary)
		{
			// Arrange
			Location locationInside{ 5, 5 };
			Boundary boundary{ 0, 10, 0, 10 }; // 10x10 square

			// Act
			bool isPointInside = Map::isLocationInsideBoundary(locationInside, boundary);

			// Assert
			Assert::IsTrue(isPointInside);
		}

		TEST_METHOD(MAP_004_IsLocationInsideBoundaryTest_PointIsOutsideBoundary)
		{
			// Arrange
			Location locationOutside{ 20, 20 };
			Boundary boundary{ 0, 10, 0, 10 }; // 10x10 square

			// Act
			bool isPointInside = Map::isLocationInsideBoundary(locationOutside, boundary);

			// Assert
			Assert::IsFalse(isPointInside);
		}

		TEST_METHOD(MAP_005_IsLocationInsisBoundaryOverlap_BoundaryOverlap)
		{
			// Arrange
			Boundary boundary1{ 0, 10, 0, 10};
			Boundary boundary2{ 5, 15, 5, 15 };

			// Act
			bool isOverlap = Map::isBoundaryOverlap(boundary1, boundary2);

			// Assert
			Assert::IsTrue(isOverlap);
		}

		TEST_METHOD(MAP_006_IsLocationInsisBoundaryOverlap_BoundaryOverlap)
		{
			// Arrange
			Boundary boundary1{ 0, 10, 10, 0 };
			Boundary boundary2{ 20, 30, 30, 20 };

			// Act
			bool isOverlap = Map::isBoundaryOverlap(boundary1, boundary2);

			// Assert
			Assert::IsFalse(isOverlap);
		}
	};
}
