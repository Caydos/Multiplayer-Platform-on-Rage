#ifndef INJECTOR_H
#define INJECTOR_H
#include <Windows.h>
#include <stdbool.h>
#include <string>

namespace Injector
{
	DWORD GetProcessIdFromName(const char* processName);

	int InjectLibraryIntoProcess(DWORD dwProcessId, const char* szLibraryPath);
}

#endif // !INJECTOR_H
