#include "Memory.h"
#include <cstring>

char* Memory::ScanPattern(char* _begin, intptr_t _size, const char* _pattern, const char* _mask)
{
    intptr_t patternLen = strlen(_mask);

    for (int i = 0; i < _size; i++)
    {
        bool found = true;
        for (int j = 0; j < patternLen; j++)
        {
            if (_mask[j] != '?' && _pattern[j] != *(char*)((intptr_t)_begin + i + j))
            {
                found = false;
                break;
            }
        }
        if (found)
        {
            return (_begin + i);
        }
    }
    return nullptr;
}