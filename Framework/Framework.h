#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <thread>
#include <shared_mutex>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <stdint.h> // types int32 int64....

#include "Logger.h"

void Kill(void);

#endif // !FRAMEWORK_H