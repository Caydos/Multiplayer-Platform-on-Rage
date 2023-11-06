#include "Encryption.h"

#pragma region Decoder
void Encryption::GetAsArguments(char**& _insertArray, unsigned int& _argCount, char* _encodedStr, char _separator)
{
	char* searchAddress = _encodedStr;
	while (true)
	{
		size_t searchAddrLength = strlen(searchAddress);
		void* result = std::memchr(searchAddress, _separator, searchAddrLength);
		if (result != nullptr)
		{
			char* position = (char*)result;
			int index = static_cast<int>(position - searchAddress);

			if (_argCount)
			{
				char** tempArray = new char* [_argCount + 1];
				std::memcpy(tempArray, _insertArray, sizeof(char*) * _argCount);
				delete[] _insertArray;
				_insertArray = tempArray;
			}
			else
			{
				_insertArray = new char* [1];
			}

			char* holder = new char[index + 1];
			std::memcpy(holder, searchAddress, index);
			holder[index] = '\0';

			_insertArray[_argCount] = holder;

			_argCount++;
			searchAddress += index + 1;
		}
		else
		{
			break;
		}
	}
}
#pragma endregion