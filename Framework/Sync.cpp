#include "Sync.h"
#include "Fibers.h"
#include "Network.h"
#include "Natives.h"


void Synchronization::Loop(void)
{
	while (true)
	{/*always send player data and each X time send entity data as encrypted tags*/
		//NativeVector3 pCoords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), false);

		//std::string callBString = Entity::GetAsJsonString();
		//if (callBString.size() > 3/*by default it will be "[]\0"*/)
		//{
		//	TriggerServerEvent("Synchronization::MainEvent", pCoords.x, pCoords.y, pCoords.z, callBString.c_str());
		//}
		Fibers::Suspend(SYNC_TIME);
	}
}

void Synchronization::NodeCallback(char** _args)//callback from server
{
	char* entArray = _args[0];
	if (entArray != nullptr)
	{
		std::cout << "Got server callback : " << entArray << std::endl;
	}
	else
	{
		std::cout << "No data to transfer" << std::endl;
	}
}

void Established(char** _args)
{
	char* message = _args[0];
	std::cout << message << std::endl;
	auto pos = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);
	auto model = ENTITY::GET_ENTITY_MODEL(PLAYER::PLAYER_PED_ID());
	Network::Events::TriggerServer("Sync::PlayerJoined", pos.x, pos.y, pos.z, model);
	Fibers::Create("SyncLoop", &Synchronization::Loop);
}

void Synchronization::PedSyncEvents(void)
{
	Network::Events::Register("Nodes::Callback", &Synchronization::NodeCallback);
	Network::Events::Register("Connections::Established", &Established);
}

