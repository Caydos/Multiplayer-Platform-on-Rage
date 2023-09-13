#ifndef EXPLORER_DIALOG_H
#define EXPLORER_DIALOG_H
#include <Windows.h>
#include <stdio.h>
#include <stdint.h>
#include <string>


#define MAX_PATH_LEN 256

namespace FileExplorer
{
	char* GetFolderPath(char* _windowName, LPCWSTR _searchTitle);

	char* GetFilePath(char* _windowName, char* _filters);
	// _filters : L"Text file\0*.txt\0" ) or _T( "3D text (*.txt)\0*.txt\0)"

	char* GetElementFromFile(const char* filename, const char* _marker);

	bool IsPathLeadingToFile(char* _path, const char* _fileName, bool _crash);
}








#endif // !EXPLORER_DIALOG_H
