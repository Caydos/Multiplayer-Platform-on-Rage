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
	this->serverId = 0;
	this->range = DEFAULT_NODE_RANGE;
	this->position = Vector3(0, 0, 0);
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
		this->entities = nullptr;
	}
	if (this->nodes != nullptr)
	{
		delete[] this->nodes;
		this->nodes = nullptr;
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
	std::shared_lock<std::shared_mutex> lock(shNodeMtx);
	this->entityCount = 0;
	if (this->entities != nullptr)
	{
		delete[] this->entities;
		this->entities = nullptr;
	}
	nodeCount = 0;
	if (this->nodes != nullptr)
	{
		delete[] this->nodes;
		this->nodes = nullptr;
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
void Node::SendUpdate(int _serverId, Vector3 _position)
{
	std::shared_lock<std::shared_mutex> lock(shNodeMtx);
	for (size_t nodeId = 0; nodeId < nodeCount; nodeId++)
	{
		if (nodes[nodeId]->serverId == _serverId)
		{
			nodes[nodeId]->position = _position;
			// callback
			std::string callBString;
			const char separator = '/';
			for (size_t entityId = 0; entityId < nodes[nodeId]->entityCount; entityId++)
			{
				if (entityId)
				{
					callBString += '|';
				}
				Entity::Entity* currentEnt = &nodes[nodeId]->entities[entityId];
				callBString += Encryption::Encode(separator, 
					currentEnt->ownerServerId,
					currentEnt->serverId,
					currentEnt->nodeCount,
					currentEnt->remove,
					currentEnt->type,
					currentEnt->task,
					currentEnt->position.x,
					currentEnt->position.y,
					currentEnt->position.z,
					currentEnt->rotations.x,
					currentEnt->rotations.y,
					currentEnt->rotations.z
				);
			}
			std::cout << callBString << std::endl;
			TriggerClientEvent(nodes[nodeId]->serverId, "Sync::NodeDataEvent", callBString.c_str());
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

	// Remove for all old nodes (will be canceled is still in range)
	for (size_t entityPtrId = 0; entityPtrId < _entity->nodeCount; entityPtrId++)
	{
		/*Editing the node entity copy so that it does not affects the main entity*/
		_entity->nodesPresence[_entity->nodeCount]->remove = true;
	}

	_entity->nodeCount = 0;
	for (size_t nodeId = 0; nodeId < nodeCount; nodeId++)
	{
		float distance = Vdist(nodes[nodeId]->position, _entity->position);
		if (distance <= nodes[nodeId]->range)
		{
			//_entity->nodesPresence[_entity->nodeCount] = nodeId;
			_entity->nodeCount++;
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
