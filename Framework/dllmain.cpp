#include "Framework.h"
#include "Network.h"
#include "Hook.h"
#include "Fibers.h"
#include "Invoker.h"
#include "GameComponents.h"
#include <WinSock2.h>

HMODULE g_hmod;
HANDLE g_uninject_thread;
char* scan_base;
size_t scan_length;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        AllocConsole();
        FILE* out;
        freopen_s(&out, "conout$", "w", stdout);
        g_hmod = hModule;

        //if (!InitSocket())
        //{
        //    Logger::Fatal(L"Fail to initiate ws2_d2.dll", L"ws2_d2 Error");
        //}

        HMODULE exe_hmod = GetModuleHandleA(NULL);
        IMAGE_DOS_HEADER* exe_dos_header;
        IMAGE_NT_HEADERS* exe_nt_header;

        exe_dos_header = (IMAGE_DOS_HEADER*)exe_hmod;
        exe_nt_header = (IMAGE_NT_HEADERS*)((char*)exe_hmod + exe_dos_header->e_lfanew);

        scan_base = (char*)exe_hmod;
        scan_length = exe_nt_header->OptionalHeader.SizeOfImage;
        Network::Events::CreateEvents();
        Hook::Initialize();
        Natives::Initialize();

        Fibers::Create("GameComponent", &GameComponents::Main);
        Fibers::Create("Events", &Network::Events::Fiber);

        break;
    }
    case DLL_PROCESS_DETACH:
        // Cleanup code: This is where you can clean up resources.
        break;
    case DLL_THREAD_ATTACH:
        // Code if a new thread is created in the process.
        break;
    case DLL_THREAD_DETACH:
        // Code if a thread exits cleanly.
        break;
    }
    return TRUE;
}

DWORD WINAPI unload_thread(LPVOID lpThreadParameter)
{
    CloseHandle(g_uninject_thread);

    //Scripting::Stop();

    FreeLibraryAndExitThread(g_hmod, 0);
}

void Kill(void)
{
    Hook::Uninitialize();
    Network::Connection::Shutdown();
    WSACleanup();
    g_uninject_thread = CreateThread(NULL, 0, &unload_thread, NULL, 0, NULL);
}

