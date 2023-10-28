#include "Header.h"
#include "Events.h"
#include "Reading.h"
#include "Sync.h"
#include "Connections.h"


#pragma region Constructors / Destructors
Synchronization::Entity::Entity()
{
	type = -1;
	task = -1;
	position = Vector3(0,0,0);
	rotations = Vector3(0, 0, 0);
}

Synchronization::Node::Node()
{
	discordId = 0;
	range = DEFAULT_NODE_RANGE;
	position.x = 0;
	position.y = 0;
	position.z = 0;
	entities = nullptr;
	entityCount = 0;
}
Synchronization::Node::~Node()
{
	if (entities != nullptr)
	{
		delete[] entities;
	}
}
#pragma endregion

void Synchronization::Node::InsertEntity(Entity* _entity)
{
	if (entityCount)
	{
		Entity** tempEnt = new Entity * [entityCount + 1];
		std::memcpy(tempEnt, entities, sizeof(Entity*) * entityCount);
		delete[] entities;
		entities = tempEnt;
	}
	else
	{
		entities = new Entity*[1];
	}
	entities[entityCount] = _entity;
	entityCount++;
}

void Synchronization::Node::Refresh(void)
{
	entityCount = 0;
	delete[]entities;
}

Synchronization::Entity** entities;
int entityCount = 0;

Synchronization::Node** nodes;
int nodeCount = 0;

void Synchronization::Entity::Update()
{
	for (size_t nodeId = 0; nodeId < nodeCount; nodeId++)
	{
		float distance = Vdist(nodes[nodeId]->position, position);
		if (distance <= nodes[nodeId]->range)
		{
			nodes[nodeId]->InsertEntity(this);
		}
	}
}

void Synchronization::AddEntity(Entity _entity)
{
	if (entityCount)
	{
		Entity** tempEnt = new Entity*[entityCount + 1];
		std::memcpy(tempEnt, entities, entityCount * sizeof(Entity*));
		delete[] entities;
		entities = tempEnt;
	}
	else
	{
		entities = new Entity*[1];
	}

	Entity* entity = new Entity;
	*entity = _entity;

	entities[entityCount] = entity;
	entityCount++;
}

void Synchronization::AddNode(std::int64_t _discordId, Vector3 _position)
{
	if (nodeCount)
	{
		Node** tempNodes = new Node*[nodeCount + 1];
		std::memcpy(tempNodes, nodes, nodeCount * sizeof(Node*));
		delete[] nodes;
		nodes = tempNodes;
	}
	else
	{
		nodes = new Node*[1];
	}
	Node* node = new Node;
	node->discordId = _discordId;
	node->position = _position;

	nodes[nodeCount] = node;
	nodeCount++;
}


void PlayerConnection(char** _args)
{
	std::int64_t id = ToInt64(_args[0]);
	unsigned int hash = ToUInt(_args[1]);
	float x = ToFloat(_args[2]);
	float y = ToFloat(_args[3]);
	float z = ToFloat(_args[4]);

	Connections::SetDiscordId(id);
	Synchronization::AddNode(id, Vector3(x,y,z));

	std::cout << "Id : " << id << std::endl;
	TriggerClientEvent(-2, "PedSync::Creation", hash, x, y, z);
}

void SyncMe(char** _args)
{
	std::int64_t id = ToInt64(_args[0]);
	float x = ToFloat(_args[1]);
	float y = ToFloat(_args[2]);
	float z = ToFloat(_args[3]);

	for (size_t nodeId = 0; nodeId < nodeCount; nodeId++)
	{
		if (nodes[nodeId]->discordId == id)
		{
			nodes[nodeId]->position = Vector3(x,y,z);
			std::cout << "Position up to date" << std::endl;
		}
	}


	//TriggerClientEvent(-2, "Way_01", id, x, y, z);
}