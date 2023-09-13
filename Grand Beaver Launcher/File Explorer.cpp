#include "File Explorer.h"
#include <tchar.h>
#include <ShlObj_core.h>
#include <atlcore.h>



char* FileExplorer::GetFolderPath(char* _windowName, LPCWSTR _searchTitle) //"Grand Beaver Launcher" "Select GtaV Game Folder"
{
    TCHAR szDir[MAX_PATH];
    BROWSEINFO bInfo;
    bInfo.hwndOwner = FindWindowA(NULL, _windowName);
    bInfo.pidlRoot = NULL;
    bInfo.pszDisplayName = szDir; // Address of a buffer to receive the display name of the folder selected by the user
    //bInfo.lpszTitle = (LPCWSTR) _searchTitle; // Title of the dialog
    bInfo.lpszTitle = _searchTitle; // Title of the dialog
    bInfo.ulFlags = 0;
    bInfo.lpfn = NULL;
    bInfo.lParam = 0;
    bInfo.iImage = -1;

    LPITEMIDLIST lpItem = SHBrowseForFolder(&bInfo);
    if (lpItem != NULL)
    {
        SHGetPathFromIDList(lpItem, szDir);
        char path[MAX_PATH];
        wcstombs(path, szDir, wcslen(szDir) + 1);

        // Replace backslashes with forward slashes in the folder path
        for (int i = 0; path[i]; i++)
        {
            if (path[i] == '\\')
            {
                path[i] = '/';
            }
        }

        return path;
    }
    return NULL;
}



char* FileExplorer::GetFilePath(char* _windowName, char* _filters)
{
    OPENFILENAME ofn = { 0 };
    TCHAR szFile[260] = { 0 };
    // Initialize remaining fields of OPENFILENAME structure
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = FindWindowA(NULL, _windowName);
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = (LPCWSTR) _filters;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE)
    {
        char path[MAX_PATH];
        wcstombs(path, ofn.lpstrFile, wcslen(ofn.lpstrFile) + 1);

        return path;
    }
    return NULL;

}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* FileExplorer::GetElementFromFile(const char* filename, const char* _marker)
{
    char* path = (char*)malloc(MAX_PATH_LEN * sizeof(char));
    if (path == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory.\n");
        return NULL;
    }

    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: Unable to open file %s.\n", filename);
        free(path);
        return NULL;
    }

    size_t markerSize = strlen(_marker);
    char line[MAX_PATH_LEN + 50];  // Add space for "Path " prefix
    while (fgets(line, MAX_PATH_LEN + 6, fp)) {  // Read lines until EOF or buffer overflow
        if (strncmp(line, _marker, markerSize-1) == 0) {  // Check if line starts with "Path "
            strncpy(path, line + markerSize, MAX_PATH_LEN);  // Copy the rest of the line (after "Path ") to the path buffer
            path[MAX_PATH_LEN - 1] = '\0';  // Ensure null termination
            break;
        }
    }

    fclose(fp);

    if (strlen(path) == 0) {  // If no path was found, return NULL
        free(path);
        return NULL;
    }

    return path;
}

//////////////////////////////////////////////////////////////////
////////////////////////// C++ 17 ////////////////////////////////
//////////////////////////////////////////////////////////////////
#include <filesystem>
#include "Logger.h"
bool FileExplorer::IsPathLeadingToFile(char* _path, const char* _fileName, bool _crash)
{
    char pathToFile[MAX_PATH];
    sprintf(pathToFile, "%s%s", _path, _fileName);
    printf("%s\n", pathToFile);
    if (!std::filesystem::exists(pathToFile))
    {
        printf("%d\n", std::filesystem::exists(pathToFile));
        if (_crash)
        {
            char* errorMsg = (char*)malloc((MAX_PATH_LEN + 21 + MAX_PATH_LEN)  * sizeof(char));
            if (errorMsg == NULL) {
                fprintf(stderr, "Error: Unable to allocate memory.\n");
                return NULL;
            }
            sprintf(errorMsg, "Unable to find file %s at %s\n%s", _fileName, _path, pathToFile);

            Logger::Fatal(errorMsg, "Wrong path");
        }
        return false;
    }
    return true;
}
