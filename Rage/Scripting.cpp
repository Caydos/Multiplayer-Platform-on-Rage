#include "pch.h"
#include "Process.h"
#include "Fiber.h"
#include "Logger.h"
#include "Scripts.h"
#include "Events.h"
#include "Sync.h"


fiber g_main_fiber;
bool IsConnected = false;

void MainScript()
{
	while (1)
	{
		if (IsConnected)
		{
			DefaultFunctions();
		}
		if (GetAsyncKeyState(VK_F5) & 0x8000)
		{
			printf("%d\n", ENTITY::GET_ENTITY_HEALTH(PLAYER::PLAYER_PED_ID()));
		}
		if (GetAsyncKeyState(VK_F8) & 0x8000)
		{
			if (!IsConnected)
			{
				IsConnected = Connect();

				if (IsConnected)
				{
					NativeVector3 vec = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);
					unsigned int hash = MISC::GET_HASH_KEY("mp_m_freemode_01");
					STREAMING::REQUEST_MODEL(hash);

					while (!STREAMING::HAS_MODEL_LOADED(hash))
					{
						Fibers::Suspend(1);
					}

					PLAYER::SET_PLAYER_MODEL(PLAYER::PLAYER_ID(), hash);
					PED::SET_PED_DEFAULT_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID());
					PED::RESET_PED_MOVEMENT_CLIPSET(PLAYER::PLAYER_PED_ID(), 0);

					TriggerServerEvent("PlayerConnection", hash, vec.x, vec.y, vec.z);
					SyncFunctions();
				}
			}
			else
			{
				//Already Connected
			}
		}
		if (GetAsyncKeyState(VK_F9) & 0x8000)
		{
			unload();
		}

		Fibers::Suspend(0);
	}
}



void init_script()
{
	Fibers::Create("Main Fiber", &MainScript);
	Fibers::Create("Event Fiber", &Events::Fiber);
	Natives::Initialize();
}

void uninit_script()
{
	Fibers::Delete();
}
