#include "Header.h"
#include "Sync.h"
#include "Connections.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace Synchronization;

std::shared_mutex shNodeMtx;
Node::Node* nodes[MAX_CONNECTIONS];
int nodeCount = 0;

#pragma region Constructors / Destructors
Node::Node::Node()
{
	this->serverId = 0;
	this->range = DEFAULT_NODE_RANGE;
	this->position = Vector3(0, 0, 0);
	this->entities = nullptr;
	this->entityCount = 0;
}
Node::Node::~Node()
{
	if (this->entities != nullptr)
	{
		delete[] this->entities;
		this->entities = nullptr;
	}
}
#pragma endregion

#pragma region NodeClass

void Node::Add(int _serverId, Vector3 _position, Entity::Entity* _owner)
{
	std::unique_lock<std::shared_mutex> lock(shNodeMtx);

	nodes[nodeCount] = new Node;
	nodes[nodeCount]->serverId = _serverId;
	nodes[nodeCount]->position = _position;

	nodeCount++;
}

void Node::Node::InsertEntity(Entity::Entity* _entity)
{
	std::shared_lock<std::shared_mutex> lock(shNodeMtx);
	std::unique_lock<std::shared_mutex> classLock(this->mutex);
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
	std::memcpy(&entities[entityCount], _entity, sizeof(Entity::Entity));
	entityCount++;
}

void Node::Node::UpdateEntity(Entity::Entity* _entity, int _entId)
{
	std::shared_lock<std::shared_mutex> lock(shNodeMtx);
	std::unique_lock<std::shared_mutex> classLock(this->mutex);
	std::memcpy(&entities[_entId], _entity, sizeof(Entity::Entity));
}


void Node::Node::Refresh(void)
{
	std::shared_lock<std::shared_mutex> lock(shNodeMtx);
	std::unique_lock<std::shared_mutex> classLock(this->mutex);
	this->entityCount = 0;
	if (this->entities != nullptr)
	{
		delete[] this->entities;
		this->entities = nullptr;
	}
}

#pragma endregion



void Node::Remove(int _serverId)
{
	std::unique_lock<std::shared_mutex> lock(shNodeMtx);
	bool foundNode = false;
	for (size_t nodeId = 0; nodeId < nodeCount; nodeId++)
	{
		if (nodes[nodeId]->serverId == _serverId)
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

#include "../Shared/Encryption.h"
#include "Events.h"
// Update node owner position
void Node::Update(int _serverId, Vector3 _position)
{
	std::shared_lock<std::shared_mutex> lock(shNodeMtx);
	for (size_t nodeId = 0; nodeId < nodeCount; nodeId++)
	{
		if (nodes[nodeId]->serverId == _serverId)
		{
			std::unique_lock<std::shared_mutex> lock(nodes[nodeId]->mutex);
			nodes[nodeId]->position = _position;
		}
	}
}

std::string Node::Node::GetAsJsonString(void)
{
	std::string str;
	if (this->entityCount)
	{
		json jsonArray = json::array();
		for (unsigned int entityId = 0; entityId < this->entityCount; entityId++)
		{
			std::cout << this->entities[entityId].ownerServerId << " " << this->serverId << std::endl;
			if (this->entities[entityId].ownerServerId != this->serverId)
			{
				json obj = this->entities[entityId].ToJson();
				jsonArray.push_back(obj);
			}
		}
		str = jsonArray.dump();
	}
	return str;
}


void Synchronization::Node::TriggerCallback(int _serverId)
{
	std::shared_lock<std::shared_mutex> lock(shNodeMtx);
	for (size_t nodeId = 0; nodeId < nodeCount; nodeId++)
	{
		if (nodes[nodeId]->serverId == _serverId)
		{
			std::shared_lock<std::shared_mutex> lock(nodes[nodeId]->mutex);
			Node* actualNode = nodes[nodeId];
			std::string str = actualNode->GetAsJsonString();
			if (str.size() > 3)
			{
				std::cout << "Callback data : " << str << std::endl;
				TriggerClientEvent(_serverId, "Sync::NodeDataEvent", str);
			}
		}
	}
}


void Node::AskForRefresh(int _serverId)
{
	std::shared_lock<std::shared_mutex> lock(shNodeMtx);
	for (size_t nodeId = 0; nodeId < nodeCount; nodeId++)
	{
		if (nodes[nodeId]->serverId == _serverId)
		{
			nodes[nodeId]->Refresh();
		}
	}
}

void Node::EntityDistCheck(Entity::Entity* _entity)
{
	std::shared_lock<std::shared_mutex> lock(shNodeMtx);

	for (size_t nodeId = 0; nodeId < nodeCount; nodeId++)
	{
		if (nodes[nodeId]->serverId != _entity->ownerServerId)
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
