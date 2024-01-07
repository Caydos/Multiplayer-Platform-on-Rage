#include "pch.h"
#include "Sync.h"
#include "Fiber.h"
#include "Events.h"
#include "Scripting.h"
#include "../Shared/Reading.h"



void Synchronization::Init(void)
{
	Fibers::Create("SyncLoop", &Synchronization::Loop);
	// Entity detection Fiber
}

void Synchronization::Loop(void)
{
	while (true)
	{/*always send player data and each X time send entity data as encrypted tags*/
		NativeVector3 pCoords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), false);

		//std::string callBString = Entity::GetAsJsonString();
		//if (callBString.size() > 3/*by default it will be "[]\0"*/)
		//{
		//	TriggerServerEvent("Synchronization::MainEvent", pCoords.x, pCoords.y, pCoords.z, callBString.c_str());
		//}
		Fibers::Suspend(SYNC_TIME);
	}
}

void Synchronization::NodeDataEvent(char** _args)//callback from server
{
	char* entArray = _args[0];
	if (entArray != nullptr)
	{
		std::cout << "Got server callback : " << entArray << std::endl;
		Entity::UpdateFromJson(entArray);
	}
	else
	{
		std::cout << "No data to transfer" << std::endl;
	}
}





void Synchronization::PlayerLanding(char** _args)
{
	playerId = ToInt(_args[0]);
	int playerPedServerId = ToInt(_args[1]);

	Entity::SetOwnerServerId(PLAYER::PLAYER_PED_ID(), playerId);
	Entity::SetServerId(PLAYER::PLAYER_PED_ID(), playerPedServerId);

	Synchronization::Init();
}

void Synchronization::PedSyncEvents(void)
{
	Events::Register("Sync::PlayerLanding", &PlayerLanding);
	Events::Register("Sync::NodeDataEvent", &Synchronization::NodeDataEvent);
}
