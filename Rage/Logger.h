#ifndef LOGGER_H
#define LOGGER_H

#include <Windows.h>

class Logger
{
private:

public:
	static void Msg(LPCWSTR _message, LPCWSTR _title);
	static void Fatal(LPCWSTR _message, LPCWSTR _title);
};


#endif // !LOGGER_H