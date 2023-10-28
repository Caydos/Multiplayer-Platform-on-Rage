#include "Header.h"
#include "Sync.h"
#include "Connections.h"


using namespace Synchronization;

std::shared_mutex shNodeMtx;
Node::Node* nodes[MAX_CONNECTIONS];
int nodeCount = 0;



#pragma region Constructors / Destructors
Node::Node::Node()
{
	this->upToDate = false;
	this->discordId = 0;
	this->range = DEFAULT_NODE_RANGE;
	this->position = Vector3(0, 0, 0);
	this->owner = nullptr;
	this->entities = nullptr;
	this->entityCount = 0;
	this->nodes = nullptr;
	this->nodeCount = 0;
}
Node::Node::~Node()
{
	if (this->entities != nullptr)
	{
		delete[] this->entities;
	}
	if (this->nodes != nullptr)
	{
		delete[] this->nodes;
	}
}
#pragma endregion

#pragma region NodeClass

Node::Node* Node::Add(std::int64_t _discordId, Vector3 _position, Entity::Entity* _owner)
{
	std::unique_lock<std::shared_mutex> lock(shNodeMtx);

	Node* node = new Node;
	node->upToDate = true;
	node->discordId = _discordId;
	node->position = _position;
	node->owner = _owner;
	
	nodes[nodeCount] = node;
	nodeCount++;

	return nodes[nodeCount];
}

void Node::Node::InsertEntity(Entity::Entity* _entity)
{
	std::shared_lock<std::shared_mutex> lock(shNodeMtx);
	if (entityCount)
	{
		Entity::Entity* tempEnt = new Entity::Entity[entityCount + 1];
		std::memcpy(tempEnt, entities, entityCount * sizeof(Entity::Entity));
		delete[] entities;
		entities = tempEnt;
	}
	else
	{
		entities = new Entity::Entity[1];
	}
	std::memcpy(&entities[entityCount], _entity, sizeof(_entity));
	entities[entityCount].needsCreation = true;
	entityCount++;
}

void Node::Node::UpdateEntity(Entity::Entity* _entity, int _entId)
{
	std::shared_lock<std::shared_mutex> lock(shNodeMtx);
	std::memcpy(&entities[_entId], _entity, sizeof(Entity::Entity));
}

void Node::Node::InsertNode(Node* _node)
{
	std::shared_lock<std::shared_mutex> lock(shNodeMtx);

	if (nodeCount)
	{
		Node** tempNodes = new Node * [nodeCount];
		std::memcpy(tempNodes, nodes, nodeCount * sizeof(Node*));
		delete[] nodes;
		nodes = tempNodes;
	}
	else
	{
		nodes = new Node * [1];
	}
	nodes[nodeCount] = _node;
	nodeCount++;
}

void Node::Node::Refresh(void)
{
	entityCount = 0;
	if (entities != nullptr)
	{
		delete[]entities;
	}
	nodeCount = 0;
	if (nodes != nullptr)
	{
		delete[]nodes;
	}
}

#pragma endregion



void Node::Remove(std::uint64_t _discordId)
{
	std::unique_lock<std::shared_mutex> lock(shNodeMtx);
	bool foundNode = false;
	for (size_t nodeId = 0; nodeId < nodeCount; nodeId++)
	{
		if (nodes[nodeId]->discordId == _discordId)
		{
			delete nodes[nodeId];
			foundNode = true;
		}
		if (foundNode && nodeId + 1 < nodeCount)
		{
			nodes[nodeId] = nodes[nodeId + 1];
		}
	}

	nodeCount--;
}

void Node::SendUpdate(std::uint64_t _discordId, Vector3 _position)
{
	std::shared_lock<std::shared_mutex> lock(shNodeMtx);
	for (size_t nodeId = 0; nodeId < nodeCount; nodeId++)
	{
		if (nodes[nodeId]->discordId == _discordId)
		{
			nodes[nodeId]->position = _position;
			//std::cout << "Position up to date" << std::endl;
		}
	}
}


void Node::EntityDistCheck(Entity::Entity* _entity)
{
	std::shared_lock<std::shared_mutex> lock(shNodeMtx);
	for (size_t nodeId = 0; nodeId < nodeCount; nodeId++)
	{
		if (nodes[nodeId]->upToDate)
		{
			float distance = Vdist(nodes[nodeId]->position, _entity->position);
			if (distance <= nodes[nodeId]->range)
			{
				bool updated = false;
				for (size_t entityId = 0; entityId < nodes[nodeId]->entityCount; entityId++)
				{
					if (nodes[nodeId]->entities[entityId].serverId == _entity->serverId)
					{
						updated = true;
						nodes[nodeId]->UpdateEntity(_entity, entityId);
					}
				}
				if (!updated)
				{
					nodes[nodeId]->InsertEntity(_entity);
				}
			}
		}
	}
}