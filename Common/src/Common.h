#pragma once

#include <string>

namespace common {
	std::string readFile(const std::string& filename);
	bool writeFile(const std::string& filename, const std::string& content);
}