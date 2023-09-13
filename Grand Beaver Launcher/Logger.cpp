#include "Logger.h"
#include <Windows.h>

void Logger::Msg(const char* _message, const char* _title)
{
	wchar_t _messageText[MAX_ERROR_LENGTH];
	mbstowcs(_messageText, _message, strlen(_message) + 1);//Plus null
	LPWSTR _messagePtr = _messageText;

	wchar_t _titleText[MAX_ERROR_LENGTH];
	mbstowcs(_titleText, _title, strlen(_title) + 1);//Plus null
	LPWSTR _titlePtr = _titleText;

	MessageBox(NULL, _messagePtr, _titlePtr, NULL);
	return;
}


void Logger::Fatal(const char* _message, const char* _title)
{
	wchar_t _messageText[MAX_ERROR_LENGTH];
	mbstowcs(_messageText, _message, strlen(_message) + 1);//Plus null
	LPWSTR _messagePtr = _messageText;

	wchar_t _titleText[MAX_ERROR_LENGTH];
	mbstowcs(_titleText, _title, strlen(_title) + 1);//Plus null
	LPWSTR _titlePtr = _titleText;

	MessageBox(NULL, _messagePtr, _titlePtr, NULL);
	TerminateProcess(GetCurrentProcess(), 0);
	return;
}



