#include "pch.h"
#include "Process.h"
#include "Hook.h"
#include "Scripting.h"

#include "Fiber.h"
#include "Threads.h"
#include "Logger.h"
#include "Events.h"
#include "API_Discord.h"

HMODULE g_hmod;
HANDLE g_uninject_thread;
char* scan_base;
size_t scan_length;

extern SOCKET clientSocket;

User userDiscord;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        AllocConsole();
        FILE* out;
        freopen_s(&out, "conout$", "w", stdout);
        g_hmod = hModule;

        if (!InitSocket())
        {
            Logger::Fatal(L"Fail to initiate ws2_d2.dll", L"ws2_d2 Error");
        }

        HMODULE exe_hmod = GetModuleHandleA(NULL);
        IMAGE_DOS_HEADER* exe_dos_header;
        IMAGE_NT_HEADERS* exe_nt_header;

        exe_dos_header = (IMAGE_DOS_HEADER*)exe_hmod;
        exe_nt_header = (IMAGE_NT_HEADERS*)((char*)exe_hmod + exe_dos_header->e_lfanew);

        scan_base = (char*)exe_hmod;
        scan_length = exe_nt_header->OptionalHeader.SizeOfImage;
        CreateEvents();
        init_script();
        Hook::Initialize();
        
        std::thread dsc(API_Discord::Check);
        dsc.detach();


        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
    {
        break;
    }
    }
    return TRUE;
}


DWORD WINAPI unload_thread(LPVOID lpThreadParameter)
{
    CloseHandle(g_uninject_thread);

    uninit_script();

    FreeLibraryAndExitThread(g_hmod, 0);
}

void unload()
{
    Hook::Uninitialize();

    closesocket(clientSocket);
    WSACleanup();
    g_uninject_thread = CreateThread(NULL, 0, &unload_thread, NULL, 0, NULL);
}

