#include "pch.h"
#include "Process.h"
#include "Hook.h"
#include "Scripting.h"

#include "Fiber.h"
#include "Threads.h"
#include "Logger.h"
#include "Events.h"

HMODULE g_hmod;
HANDLE g_uninject_thread;
char* scan_base;
size_t scan_length;

extern SOCKET clientSocket;

#include <array>
#include <cassert>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <vector>

#include <discord.h>


struct DiscordState {
    discord::User currentUser;

    std::unique_ptr<discord::Core> core;
};

namespace {
    volatile bool interrupted{ false };
}


int dickChop()
{
    DiscordState state{};

    discord::Core* core{};
    auto result = discord::Core::Create(1153788166487162901, DiscordCreateFlags_Default, &core);
    state.core.reset(core);
    if (!state.core) {
        std::cout << "Failed to instantiate discord core! (err " << static_cast<int>(result)
            << ")\n";
        std::exit(-1);
    }


    core->UserManager().OnCurrentUserUpdate.Connect([&state]() {
        state.core->UserManager().GetCurrentUser(&state.currentUser);

        std::cout << "Current user updated: " << state.currentUser.GetUsername() << "#" << state.currentUser.GetDiscriminator() << "\n";


        });

    //state.core->ActivityManager().RegisterCommand("run/command/foo/bar/baz/here.exe");
    //state.core->ActivityManager().RegisterSteam(123123321);


    discord::Activity activity{};
    activity.SetDetails("Hanging around");
    activity.SetState("Players Connected");
    activity.GetAssets().SetSmallImage("gtaV");
    activity.GetAssets().SetSmallText("Grand theft auto V");

    activity.GetAssets().SetLargeImage("banner");
    activity.GetAssets().SetLargeText("Christmas event");

    activity.GetSecrets().SetJoin("https://www.google.com/");
    activity.GetParty().GetSize().SetCurrentSize(1);
    activity.GetParty().GetSize().SetMaxSize(700);
    activity.GetParty().SetId("party id");
    activity.GetParty().SetPrivacy(discord::ActivityPartyPrivacy::Public);
    activity.SetType(discord::ActivityType::Playing);
    state.core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {
        std::cout << ((result == discord::Result::Ok) ? "Succeeded" : "Failed")
            << " updating activity!\n";
        });



    std::signal(SIGINT, [](int) { interrupted = true; });

    do {
        state.core->RunCallbacks();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    } while (!interrupted);

    return 0;
}


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
        
        std::thread dsc(dickChop);
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

