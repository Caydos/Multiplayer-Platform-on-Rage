#include "Reading.h"

int ToInt(const char* _array)
{
	if (_array != nullptr)
	{
		return std::atoi(_array);
	}
	else
	{
		return 0;
	}
}

std::int64_t ToInt64(const char* _array)
{
	if (_array != nullptr)
	{
		std::int64_t result = *(int64_t*)_array;
		return result;
	}
	else
	{
		return 0;
	}
}

int ToUInt(const char* _array)
{
	if (_array != nullptr)
	{
		return std::atoi(_array);
	}
	else
	{
		return 0;
	}
}

char ToChar(const char* _array)
{
	if (_array != nullptr)
	{
		return std::atoi(_array);
	}
	else
	{
		return 0;
	}
}

float ToFloat(const char* _array)
{
	if (_array != nullptr)
	{
		return std::atof(_array);
	}
	else
	{
		return 0;
	}
}

double ToDouble(const char* _array)
{
	if (_array != nullptr)
	{
		return std::atof(_array);
	}
	else
	{
		return 0;
	}
}

bool ToBool(const char* _array)
{
	if (_array != nullptr)
	{
		return std::atoi(_array);
	}
	else
	{
		return false;
	}
}

char* ToString(const char* _array)
{
	if (_array != nullptr)
	{
		return (char*)_array;
	}
	else
	{
		return nullptr;
	}
}