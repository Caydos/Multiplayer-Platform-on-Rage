#ifndef ENCRYPTION_H
#define ENCRYPTION_H

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
}


#endif // !ENCRYPTION_H
