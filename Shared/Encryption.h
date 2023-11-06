#ifndef ENCRYPTION_H
#define ENCRYPTION_H
#include <cstring>
#include <sstream>

namespace Encryption
{
#pragma region Encoder
	template<typename Type>
	void AddValueToString(std::string& _string, char _separator, Type _value)
	{
		std::stringstream valueString;
		valueString << _value;
		valueString << _separator;
		_string += valueString.str();
	}
	template<typename ...Type>
	std::string Encode(char _separator, Type ..._values)
	{
		std::string encodedString;
		(AddValueToString(encodedString, _separator, _values), ...);
		return encodedString;
	}
#pragma endregion

#pragma region Decoder
	void GetAsArguments(char**& _insertArray, unsigned int& _argCount, char* _encodedStr, char _separator);
#pragma endregion
}


#endif // !ENCRYPTION_H
