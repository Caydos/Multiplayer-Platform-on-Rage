#ifndef READING_H
#define READING_H
#include <cstdint>

int ToInt(const char* _array);
std::int64_t ToInt64(const char* _array);

int ToUInt(const char* _array);

char ToChar(const char* _array);

float ToFloat(const char* _array);

double ToDouble(const char* _array);

bool ToBool(const char* _array);

char* ToString(const char* _array);

#endif // !READING_H
