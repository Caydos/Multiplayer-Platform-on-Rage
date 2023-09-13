#ifndef DATA_HOLDER_H
#define DATA_HOLDER_H
#include "Header.h"

class DataT
{
public:
	int* integers = nullptr;
	unsigned int intCount = 0;

	unsigned int* unsigned_integers = nullptr;
	unsigned int unsigned_intCount = 0;

	char* characters = nullptr;
	unsigned int charactersCount = 0;

	float* flotants = nullptr;
	unsigned int floatCount = 0;

	double* doubles = nullptr;
	unsigned int doubleCount = 0;

	bool* booleans = nullptr;
	unsigned int boolCount = 0;

	char** strings = nullptr;
	unsigned int stringCount = 0;


	void* AddString(const char* array)
	{
		char** temp = new char* [stringCount + 1];

		for (int i = 0; i < stringCount; i++)
		{
			temp[i] = strings[i];
		}

		temp[stringCount] = new char[strlen(array) + 1];
		strcpy(temp[stringCount], array);

		delete[] strings;

		strings = temp;
		stringCount++;
		return &strings[stringCount - 1];
	}


	void Reset(void)
	{
		if (integers != nullptr)
		{
			free(integers);
			integers = nullptr;
		}
		intCount = 0;
		if (unsigned_integers != nullptr)
		{
			free(unsigned_integers);
			unsigned_integers = nullptr;
		}
		unsigned_intCount = 0;
		if (characters != nullptr)
		{
			free(characters);
			characters = nullptr;
		}
		charactersCount = 0;
		if (integers != nullptr)
		{
			free(flotants);
			flotants = nullptr;
		}
		floatCount = 0;
		if (doubles != nullptr)
		{
			free(doubles);
			doubles = nullptr;
		}
		doubleCount = 0;
		if (booleans != nullptr)
		{
			free(booleans);
			booleans = nullptr;
		}
		boolCount = 0;
		if (strings != nullptr)
		{
			for (unsigned int i = 0; i < stringCount; i++)
			{
				if (strings[i] != nullptr)
				{
					delete[] strings[i];
				}
			}
			if (strings != nullptr)
			{
				delete[] strings;
				strings = nullptr;
			}
		}
		stringCount = 0;
	}

};

#endif // !DATA_HOLDER_H
