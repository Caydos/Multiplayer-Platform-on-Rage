#include "Injector.h"

#include <tlhelp32.h>


namespace Injector
{
	DWORD GetProcessIdFromName(const char* processName)
	{
		// Convert the process name to a wide-character string
		std::wstring wideProcessName(processName, processName + strlen(processName));

		// Create a snapshot of all running processes
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		// Initialize the process entry structure
		PROCESSENTRY32 processEntry;
		processEntry.dwSize = sizeof(processEntry);

		// Iterate through the list of processes and find the one with a matching name
		BOOL found = Process32First(snapshot, &processEntry);
		while (found)
		{
			if (_wcsicmp(processEntry.szExeFile, wideProcessName.c_str()) == 0)
			{
				// Close the snapshot handle and return the process ID
				CloseHandle(snapshot);
				return processEntry.th32ProcessID;
			}
			found = Process32Next(snapshot, &processEntry);
		}

		// Close the snapshot handle and return 0 if the process wasn't found
		CloseHandle(snapshot);
		return 0;
	}

	int InjectLibraryIntoProcess(HANDLE hProcess, const char* szLibraryPath)
	{
		int iReturn = 0;

		// Get the length of the library path
		size_t sLibraryPathLen = (strlen(szLibraryPath) + 1);

		// Allocate the a block of memory in our target process for the library path
		void* pRemoteLibraryPath = VirtualAllocEx(hProcess, NULL, sLibraryPathLen, MEM_COMMIT, PAGE_READWRITE);

		// Write our library path to the allocated block of memory
		SIZE_T sBytesWritten = 0;
		WriteProcessMemory(hProcess, pRemoteLibraryPath, (void*)szLibraryPath, sLibraryPathLen, &sBytesWritten);

		if (sBytesWritten != sLibraryPathLen)
		{
			// Failed to write the library path
			iReturn = 1;
		}
		else
		{
			// Get the handle of Kernel32.dll
			HMODULE hKernel32 = GetModuleHandle(L"Kernel32");

			// Get the address of the LoadLibraryA function from Kernel32.dll
			FARPROC pfnLoadLibraryA = GetProcAddress(hKernel32, "LoadLibraryA");

			// Create a thread inside the target process to load our library
			HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pfnLoadLibraryA, pRemoteLibraryPath, 0, NULL);

			if (hThread)
			{
				// Wait for the created thread to end
				WaitForSingleObject(hThread, INFINITE);

				// Close our thread handle
				CloseHandle(hThread);
			}
			else
			{
				// Thread creation failed
				iReturn = 2;
			}
		}

		// Free the allocated block of memory inside the target process
		VirtualFreeEx(hProcess, pRemoteLibraryPath, sizeof(pRemoteLibraryPath), MEM_RELEASE);
		return iReturn;
	}

	int InjectLibraryIntoProcess(DWORD dwProcessId, const char* szLibraryPath)
	{
		// Open our target process
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);

		if (!hProcess)
		{
			// Failed to open the process
			return 3;
		}

		// Inject the library into the process
		int iReturn = InjectLibraryIntoProcess(hProcess, szLibraryPath);

		// Close the process handle
		CloseHandle(hProcess);
		return iReturn;
	}
}
//
//bool Injector::InjectDLL(const int& pid, const std::string& DLL_Path)
//{
//	long dll_size = DLL_Path.length() + 1;
//	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
//
//	if (hProc == NULL)
//	{
//		printf("[!]Fail to open target process!");
//		return false;
//	}
//	printf("[+]Opening Target Process...");
//
//	LPVOID MyAlloc = VirtualAllocEx(hProc, NULL, dll_size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
//	if (MyAlloc == NULL)
//	{
//		printf("[!]Fail to allocate memory in Target Process.");
//		return false;
//	}
//	printf("[+]Allocating memory in Target Process.");
//
//	int IsWriteOK = WriteProcessMemory(hProc, MyAlloc, DLL_Path.c_str(), dll_size, 0);
//	if (IsWriteOK == 0)
//	{
//		printf("[!]Fail to write in Target Process memory.");
//		return false;
//	}
//	printf("[+]Creating Remote Thread in Target Process");
//
//	DWORD dWord;
//	LPTHREAD_START_ROUTINE addrLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(LoadLibrary(L"kernel32"), "LoadLibraryA");
//	HANDLE ThreadReturn = CreateRemoteThread(hProc, NULL, 0, addrLoadLibrary, MyAlloc, 0, &dWord);
//	if (ThreadReturn == NULL)
//	{
//		printf("[!]Fail to create Remote Thread");
//		return false;
//	}
//
//	if ((hProc != NULL) && (MyAlloc != NULL) && (IsWriteOK != ERROR_INVALID_HANDLE) && (ThreadReturn != NULL))
//	{
//		printf("[+]DLL Successfully Injected :)");
//		return true;
//	}
//
//	return false;
//}