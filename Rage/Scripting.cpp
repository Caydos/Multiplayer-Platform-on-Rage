#include "pch.h"
#include "Scripting.h"
#include "Natives.h"
#include "Fiber.h"

#include "Scripts.h"
#include "Events.h"
#include "Sync.h"
#include "API_Discord.h"
#include "Pools.h"
#include "PedFactory.h"

int playerId = -1;
bool IsConnected = false;
bool loading = true;

void Scripting::Main()
{
	while (1)
	{
		if (!loading)
		{
			DefaultFunctions();
		}
		else
		{
			loading = DLC::GET_IS_LOADING_SCREEN_ACTIVE();
			if (loading)
			{
				MISC::SET_INSTANCE_PRIORITY_MODE(1);
				DLC::ON_ENTER_SP();
				//Disable Stunt Jumps
				for (int i = 0; i < 50; i++)
				{
					MISC::DISABLE_STUNT_JUMP_SET(i);
					MISC::DELETE_STUNT_JUMP(i);
				}
				//Disable Hospitals
				//for (int i = 0; i <= 5; i++)
				//{
				//	MISC::DISABLE_HOSPITAL_RESTART(i, true);
				//}
				// Disable save option
				MISC::SET_MISSION_FLAG(true);
			}
			else
			{
				Pools::Hook();
				PedFactory::Hook();
			}
		}
		if (GetAsyncKeyState(VK_F8) & 0x8000)
		{
			if (!IsConnected)
			{
				IsConnected = Connect();

				if (IsConnected)
				{
					NativeVector3 vec = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);
					Hash hash = MISC::GET_HASH_KEY("mp_m_freemode_01");
					STREAMING::REQUEST_MODEL(hash);

					while (!STREAMING::HAS_MODEL_LOADED(hash))
					{
						Fibers::Suspend(1);
					}

					PLAYER::SET_PLAYER_MODEL(PLAYER::PLAYER_ID(), hash);
					PED::SET_PED_DEFAULT_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID());
					PED::RESET_PED_MOVEMENT_CLIPSET(PLAYER::PLAYER_PED_ID(), 0);
					
					Synchronization::Entity::Entity* playerEnt = new Synchronization::Entity::Entity;
					playerEnt->localId = PLAYER::PLAYER_PED_ID();
					playerEnt->type = 1;
					playerEnt->hash = hash;

					Synchronization::Entity::Add(playerEnt);

					TriggerServerEvent("PlayerConnection", API_Discord::GetUser().id, hash, vec.x, vec.y, vec.z);
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



void Scripting::Init()
{
	Fibers::Create("Main Fiber", &Scripting::Main);
	Fibers::Create("Event Fiber", &Events::Fiber);
	Natives::Initialize();
}

void Scripting::Stop()
{
	Fibers::Delete();
}
