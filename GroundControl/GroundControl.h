#pragma once

#include "Map.h"

namespace groundControl {
	class GroundControl {
	public:
		GroundControl();
		GroundControl(int id);

	private:
		common::Location location;
		common::Boundary boundary;
	};
}