#include "Header.h"
#include "SyncEvents.h"
#include "Sync.h"
#include "Reading.h"
#include "Connections.h"

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

	TriggerClientEvent(serverId, "PlayerLanding", serverId);
}

void Synchronization::MainEvent(char** _args)
{
#pragma region Reception

	int serverId = Connections::GetLocalThreadId();
	float x = ToFloat(_args[0]);
	float y = ToFloat(_args[1]);
	float z = ToFloat(_args[2]);

	Node::SendUpdate(serverId, Vector3(x, y, z));

	/* for each entity in xml tag (player ped included) add them into nodes*/
	/* for k ent, do ent->NodeInsertion()*/

#pragma endregion
#pragma region Callback

	//send to client node content
	//TriggerClientEvent(Connections::GetLocalThreadId(), "Way_01", x, y, z);

	// Refresh node
	Node::AskForRefresh(serverId);

#pragma endregion
}

