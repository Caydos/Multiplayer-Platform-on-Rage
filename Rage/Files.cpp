#include "Files.h"
#include <iostream>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;
std::vector<std::string> Files::GetAllAtPath(const char* _path)
{
	std::vector<std::string> paths;

	try {
		if (fs::exists(_path) && fs::is_directory(_path))
		{
			for (const auto& entry : fs::directory_iterator(_path))
			{
				paths.push_back(entry.path().filename().generic_string());
			}
		}
		else
		{
			std::cout << "[Files::GetAllAtPath] - Unable to locate folder : " << _path << std::endl;
		}
	}
	catch (const fs::filesystem_error& err)
	{
		std::cerr << "[Files::GetAllAtPath] - Filesystem error: " << err.what() << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << "[Files::GetAllAtPath] - Standard exception: " << e.what() << std::endl;
	}
	return paths;
}

std::string Files::GetFileContent(const char* _path)
{
	std::string content;
	std::ifstream file(_path);

	if (!file.is_open()) {
		std::cerr << "Error opening file at path : " << _path << std::endl;
		return content;
	}

	std::string line;
	while (std::getline(file, line)) {
		content += line;
	}

	file.close();
	return content;
}

void Files::Create(const char* _path, const char* _name, const char* _extension, const char* _content)
{
	std::string fileName = _path;
	fileName += _name;
	fileName += _extension;

	std::ofstream file;

	// Open the file with the given filename
	file.open(fileName);

	// Check if the file is open
	if (file.is_open()) {
		// Write the content to the file
		file << _content;

		// Close the file
		file.close();
	}
	else {
		// Handle the case where the file couldn't be opened
		std::cerr << "Unable to open file: " << fileName << std::endl;
	}
}
