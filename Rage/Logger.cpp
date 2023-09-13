#include "pch.h"
#include "Logger.h"


void Logger::Msg(LPCWSTR _message, LPCWSTR _title)
{
	MessageBox(NULL, _message, _title, NULL);
	return;
}


void Logger::Fatal(LPCWSTR _message, LPCWSTR _title)
{
	MessageBox(NULL, _message, _title, NULL);
	TerminateProcess(GetCurrentProcess(), 0);
	return;
}



