#ifndef LOGGER_H
#define LOGGER_H
#include <iostream>
#include <fstream>
#include <string>

namespace Logger
{
	std::ofstream& GetFile();

	template<typename ...Type>
	void Write(Type..._args)
	{
		//std::ofstream file;
		//if (!file.is_open())
		//{
		//	file.open("log.txt", std::ios::app);
		//}

		(std::cout << ... << _args);

		//file.close();
	}
}

#endif // !LOGGER_H