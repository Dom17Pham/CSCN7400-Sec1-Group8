#pragma once

#include <string>

namespace common {
	struct Location {
		double x;
		double y;
	};

	std::string ReadFile(const std::string& filename);
	bool WriteFile(const std::string& filename, const std::string& content);
}