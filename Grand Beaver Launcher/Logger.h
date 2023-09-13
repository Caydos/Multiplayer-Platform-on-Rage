#ifndef LOGGER_H
#define LOGGER_H

#define MAX_ERROR_LENGTH 255

class Logger
{
private:

public:
	static void Msg(const char* _message, const char* _title);
	static void Fatal(const char* _message, const char* _title);
};


#endif // !LOGGER_H