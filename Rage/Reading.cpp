#include "pch.h"
#include "Reading.h"


int ToInt(const char* _array)
{
	return std::atoi(_array + 1);
}

int ToUInt(const char* _array)
{
	return std::atoi(_array + 1);
}

char ToChar(const char* _array)
{
	return std::atoi(_array + 1);
}

float ToFloat(const char* _array)
{
	return std::atof(_array + 1);
}

double ToDouble(const char* _array)
{
	return std::atof(_array + 1);
}

bool ToBool(const char* _array)
{
	return std::atoi(_array + 1);
}

char* ToString(const char* _array)
{
	if (_array[0] == 'P')
	{//pointers
		if (_array[1] == 'K' && _array[2] == 'c')
		{
			return (char*)_array + 3;
		}
		else
		{
			return (char*)_array + 2;
		}
	}
	return NULL;
}