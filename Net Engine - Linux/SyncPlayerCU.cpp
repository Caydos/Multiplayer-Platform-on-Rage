#include "Header.h"
#include "SyncEvents.h"
#include "Sync.h"
#include "Connections.h"
#include "../Shared/Reading.h"
#include "../Shared/Encryption.h"

using namespace Synchronization;

void PlayerConnection(char** _args)
{
	int serverId = Connections::GetLocalThreadId();
	std::int64_t discordId = ToInt64(_args[0]);
	unsigned int hash = ToUInt(_args[1]);
	float x = ToFloat(_args[2]);
	float y = ToFloat(_args[3]);
	float z = ToFloat(_args[4]);

	Connections::SetDiscordId(discordId);

	Entity::Entity* playerEnt = new Entity::Entity;
	playerEnt->position = Vector3(x, y, z);
	playerEnt->ownerServerId = serverId;

	Node::Add(serverId, playerEnt->position, playerEnt);

	Entity::Add(playerEnt);

	playerEnt->NodeInsertion();

	TriggerClientEvent(serverId, "Sync::PlayerLanding", serverId);
}

void Synchronization::MainEvent(char** _args)
{
#pragma region Reception

	int serverId = Connections::GetLocalThreadId();
	float x = ToFloat(_args[0]);
	float y = ToFloat(_args[1]);
	float z = ToFloat(_args[2]);
	char* entArray = _args[3];

	Node::SendUpdate(serverId, Vector3(x, y, z));

	if (entArray != nullptr)
	{
		char** entArrays = nullptr;
		unsigned int entCount = 0;
		Encryption::GetAsArguments(entArrays, entCount, entArray, '|');

		for (size_t i = 0; i < entCount; i++)
		{
			char** entArray = nullptr;
			unsigned int attributesCount = 0;
			Encryption::GetAsArguments(entArray, attributesCount, entArrays[i], '/');
			std::cout << "Entity buffer : " << entArrays[i] << std::endl;
		}
		/* for each entity in xml tag (player ped included) add them into nodes*/
		/* for k ent, do ent->NodeInsertion()*/
	}

#pragma endregion
#pragma region Callback

	//send to client node content
	//TriggerClientEvent(Connections::GetLocalThreadId(), "Way_01", x, y, z);

	// Refresh node
	Node::AskForRefresh(serverId);

#pragma endregion
}

