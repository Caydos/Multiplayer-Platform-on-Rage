#define _CRT_SECURE_NO_WARNINGS

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

int main(int, char**)
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

    state.core->ActivityManager().RegisterCommand("run/command/foo/bar/baz/here.exe");
    state.core->ActivityManager().RegisterSteam(123123321);


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
