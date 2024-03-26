#include "Common.h"

#include <iostream>
#include <fstream>

namespace common {
	std::string ReadFile(const std::string& filename) {
		std::string content = "";
		std::ifstream file(filename, std::ifstream::in);
		if (file.is_open()) {
			std::string line;
			while (std::getline(file, line)) {
				content += line;
			}
			file.close();
		}
		//If file did not open, meaning it has an error, it will return empty string.
		return content;
	}

	bool WriteFile(const std::string& filename, const std::string& content) {
		bool isWritten = false;
		std::ofstream file(filename, std::ofstream::out);
		if (file.is_open()) {
			file << content;
			file.close();
			isWritten = true;
		}
		return isWritten;
	}
}