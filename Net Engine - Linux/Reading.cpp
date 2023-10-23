#include "Header.h"
#include "Reading.h"


int ToInt(const char* _array)
{
	return std::atoi(_array/* + 2*/);
}

std::int64_t ToInt64(const char* _array)
{
	std::int64_t result = *(int64_t*)_array;
	return result;
}

int ToUInt(const char* _array)
{
	return std::atoi(_array/* + 2*/);
}

char ToChar(const char* _array)
{
	return std::atoi(_array/* + 2*/);
}

float ToFloat(const char* _array)
{
	return std::atof(_array/* + 2*/);
}

double ToDouble(const char* _array)
{
	return std::atof(_array/* + 2*/);
}

bool ToBool(const char* _array)
{
	return std::atoi(_array/* + 3*/);
}

char* ToString(const char* _array)
{
	return (char*)_array/* + 5*/;
}