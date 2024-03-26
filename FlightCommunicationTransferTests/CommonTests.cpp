#include "CppUnitTest.h"

#include "Common.h"
#include <fstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace FlightCommunicationTransferTests
{
	TEST_CLASS(CommonTests)
	{
	public:

		TEST_METHOD(CMN_001_ReadFileTest)
		{
			// Arrange
			std::string filename = "testfile.txt";
			std::ofstream outfile(filename);
			outfile << "This is a test file content.";
			outfile.close();

			// Act
			std::string content = common::ReadFile(filename);

			// Assert
			Assert::AreEqual(std::string("This is a test file content."), content);

			// Clean up - delete the file
			std::remove(filename.c_str());
		}

		TEST_METHOD(CMN_002_WriteFileTest)
		{
			// Arrange
			std::string filename = "testfile.txt";
			std::string data = "This is test data to write to file.";

			// Act
			bool result = common::WriteFile(filename, data);

			// Assert
			Assert::IsTrue(result);

			// Clean up - delete the file
			std::remove(filename.c_str());
		}
	};
}
