#include <stdio.h>
#include <stdint.h>
#include "File Explorer.h"
#include "Logger.h"
#include "Injector.h"
#include <fstream>
#include <iostream>

void RunGame(void);
void InjectDll(void);

char appName[22] = "Grand Beaver Launcher";
char* gamePath{};
char Folder[MAX_PATH];
#define LIBRARY_EXTENSION ".dll"
#define INFO_CLIENT_CORE	"Rage" LIBRARY_EXTENSION

int main(void)
{
	AllocConsole();
	FILE* out;
	freopen_s(&out, "conout$", "w", stdout);
	FILE* dataFile;
	if (fopen_s(&dataFile, "data.cfg", "r") != 0)
	{
		gamePath = FileExplorer::GetFolderPath(appName, L"Select GtaV game folder");
		printf("%s\n", gamePath);
        if (gamePath != NULL)
        {
			FileExplorer::IsPathLeadingToFile(gamePath, "/gtavlauncher.exe", true);
			FILE* dataFileCreation;
			fopen_s(&dataFileCreation, "data.cfg", "w+");
            fprintf(dataFileCreation, "Path %s", gamePath);
			fclose(dataFileCreation);
        }
		else
		{
			Logger::Fatal("Failed to Select GtaV game Folder", "Game folder selection");
		}
	}
	else
	{
		gamePath = FileExplorer::GetElementFromFile("data.cfg", "Path ");
		if (gamePath == NULL)
		{
			Logger::Fatal("Unable to find Game Folder Path in file", "Game Folder Path");
		}
		fclose(dataFile);
	}


	RunGame();
	printf("\n");

	while (true)
	{
		char msg[100];
		std::cin.getline(msg, 100);
		if (strcmp(msg, "restart") == 0)
		{
			RunGame();
			printf("\n");
		}
		else if (strcmp(msg, "quit") == 0)
		{
			break;
		}
	}
	//system("pause");
	return EXIT_SUCCESS;
}



void RunGame(void)
{
	char commandFilePath[MAX_PATH];
	sprintf(commandFilePath, "%s%s", gamePath, "/commandline.txt");
	for (int i = 0; commandFilePath[i]; i++)
	{
		if (commandFilePath[i] == '/')
		{
			commandFilePath[i] = '\\';
		}
	}

	FILE* commandFile;
	if (fopen_s(&commandFile, commandFilePath, "r") != 0)
	{
		printf("Creating command.txt file at path : %s\n", commandFilePath);
		FILE* commandFileCreation;
		printf("%d\n",fopen_s(&commandFileCreation, commandFilePath, "w+"));
		fprintf(commandFileCreation, "-scOfflineOnly");
		fclose(commandFileCreation);
	}
	else
	{
		fclose(commandFile);
	}

	char runCommand[MAX_PATH + 10];
	sprintf(runCommand, "explorer %s%s", gamePath, "\\gtavlauncher.exe");

	for (int i = 0; runCommand[i]; i++)
	{
		if (runCommand[i] == '/')
		{
			runCommand[i] = '\\';
		}
	}
	//std::cout << runCommand << std::endl;
	system(runCommand);

	// Initialisation
	GetCurrentDirectoryA(sizeof(Folder), Folder);
	InjectDll();
}

//unsigned int dllCount = 2;
//const char* dllNames[] = {"discord_game_sdk","Rage"};
unsigned int dllCount = 1;
const char* dllNames[] = { "Framework" };

void InjectDll(void)
{
	bool Started = false;
	while (!Started)
	{
		int GamePID = Injector::GetProcessIdFromName("GTA5.exe");
		if (GamePID != 0)
		{
			DWORD ForePID;

			SetForegroundWindow(FindWindow(NULL, L"Grand Theft Auto V"));

			HWND GameWindow = GetForegroundWindow();
			GetWindowThreadProcessId(GameWindow, &ForePID);
			SetWindowTextA(GameWindow, appName);

			if (GamePID == ForePID && IsWindowVisible(GameWindow))
			{
				for (int i = 0; i < dllCount; i++)
				{
					const char* extension = ".dll";
					int folderLength = strlen(Folder);
					int dllNameLength = strlen(dllNames[i]);
					int extensionLength = strlen(extension);

					char* dllPath = new char[folderLength + dllNameLength + extensionLength + 2];

					std::memcpy(dllPath, Folder, folderLength);
					dllPath[folderLength] = '\\';
					std::memcpy(dllPath + folderLength + 1, dllNames[i], dllNameLength);

					std::memcpy(dllPath + folderLength + dllNameLength + 1, extension, extensionLength);
					dllPath[folderLength + dllNameLength + extensionLength + 1] = '\0';
					//char DllPath[MAX_PATH];
					//sprintf_s(DllPath, "%s\\" "Rage.dll", Folder);
					std::cout << "Name is :" << dllPath << std::endl;
					int iReturn = Injector::InjectLibraryIntoProcess(GamePID, dllPath);
					//switch (iReturn)
					//{
					//case 0:
					//{
					//	std::cout << dllPath << std::endl;
					//	Started = true;
					//}
					//case 1:
					//{
					//	Logger::Fatal("Failed to write library path into remote process.", "Injection");
					//	break;
					//}
					//case 2:
					//{
					//	Logger::Fatal("Failed to create remote thread in remote process.", "Injection");
					//	break;
					//}
					//case 3:
					//{
					//	Logger::Fatal("Failed to open the remote process.", "Injection");
					//	break;
					//}
					//default:
					//{
					//	printf("Succeffuly injected\n");
					//	Started = true;
					//}
					//}
					std::cout << iReturn << std::endl;
					delete[] dllPath;
				}
				break;
			}
		}
		Sleep(500);
	}
}