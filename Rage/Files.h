#ifndef FILES_H
#define FILES_H
#include <vector>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Files
{
	std::vector<std::string> GetAllAtPath(const char* _path);
	std::string GetFileContent(const char* _path);
	void Create(const char* _path, const char* _name, const char* _extension, const char* _content);
}

#endif // !FILES_H