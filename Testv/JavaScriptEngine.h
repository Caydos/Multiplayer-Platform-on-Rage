#ifndef JAVASCRIPT_ENGINE
#define JAVASCRIPT_ENGINE

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

namespace JsEngine
{
	std::string FileToString(const std::string& _filename);
	void Initialize(void);
	void Dispose(void);
}


#endif // !JAVASCRIPT_ENGINE