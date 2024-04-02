#include "GroundControl.h"

namespace groundControl {
	GroundControl::GroundControl() : location{ 0.0, 0.0 }, boundary{ 0.0,0.0,0.0,0.0 } {}

	GroundControl::GroundControl(int id) : GroundControl()
	{
		location = common::groundControlLocation[id];
		boundary = common::Map::GetBoundaryFromLocationAndExtends(location, common::extends);
	}
}
