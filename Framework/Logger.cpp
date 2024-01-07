#include "Logger.h"

std::ofstream file;

std::ofstream& Logger::GetFile()
{
    if (!file.is_open())
    {
        file.open("log.txt", std::ios::app);
    }
    return file;
}
