#include "Header.h"
#include "Sync.h"

using namespace Synchronization;

std::shared_mutex shEntMtx;
Entity::Entity** entities;
int entityCount = 0;

#pragma region Constructors / Destructors

Entity::Entity::Entity()
{
	this->ownerServerId = -1;
	this->serverId = -1;
	this->nodeCount = 0;
	this->type = -1;
	this->task = -1;
	this->position = Vector3(0,0,0);
	this->rotations = Vector3(0, 0, 0);
}

Entity::Entity::~Entity()
{

}
#pragma endregion


void Synchronization::Entity::Entity::NodeInsertion(void)
{
	std::shared_lock<std::shared_mutex> lock(shEntMtx);
	this->nodeCount = 0;
	Node::EntityDistCheck(this);
	/*check last update time to free some memory*/
}

void Entity::Add(Entity* _entity)
{
	std::unique_lock<std::shared_mutex> lock(shEntMtx);
	if (entities != nullptr)
	{
		Entity** tempEnt = new Entity * [entityCount + 1];
		std::memcpy(tempEnt, entities, entityCount * sizeof(Entity));
		delete[] entities;
		entities = tempEnt;
	}
	else
	{
		entities = new Entity * [1];
	}
	entities[entityCount] = _entity;
	entities[entityCount]->serverId = entityCount;
	entityCount++;
}


void Entity::Remove(int _serverId)
{

}


