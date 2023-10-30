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
	std::memcpy(&entities[entityCount], _entity, sizeof(_entity));
	std::cout << "[Insertion] - Entity position x : " << entities[entityCount].position.x << std::endl;
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
	std::cout << this->entityCount << " entities in node" << std::endl;
	this->entityCount = 0;
	if (this->entities != nullptr)
	{
		delete[] this->entities;
		this->entities = nullptr;
	}
	nodeCount = 0;
	if (this->nodes != nullptr)
	{
		std::cout << "Deleting something it shoudln't" << std::endl;
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

// Update node owner position & send it's entity to other nodes
void Node::SendUpdate(int _serverId, Vector3 _position)
{
	std::shared_lock<std::shared_mutex> lock(shNodeMtx);
	for (size_t nodeId = 0; nodeId < nodeCount; nodeId++)
	{
		if (nodes[nodeId]->serverId == _serverId)/*not my entity*/
		{
			nodes[nodeId]->position = _position;
			nodes[nodeId]->Refresh();
		}
	}
}


void Node::EntityDistCheck(Entity::Entity* _entity)
{
	std::shared_lock<std::shared_mutex> lock(shNodeMtx);
	for (size_t nodeId = 0; nodeId < nodeCount; nodeId++)
	{
		float distance = Vdist(nodes[nodeId]->position, _entity->position);
		if (distance <= nodes[nodeId]->range)
		{
			_entity->nodeCount++;
			bool updated = false;
			for (size_t entityId = 0; entityId < nodes[nodeId]->entityCount; entityId++)
			{
				if (nodes[nodeId]->entities[entityId].serverId == _entity->serverId)
				{
					updated = true;
					std::cout << "updating into a node" << std::endl;
					nodes[nodeId]->UpdateEntity(_entity, entityId);
				}
			}
			if (!updated)
			{
				std::cout << "inserting into a node" << std::endl;
				nodes[nodeId]->InsertEntity(_entity);
			}
		}
	}
}
