#include "Sync.h"
#include <cstring>

Synchronization::Entity::Entity** entities;
int entityCount = 0;
unsigned int entityCountHistory = 0;

#pragma region Constructors / Destructors

Synchronization::Entity::Entity::Entity()
{
	this->ownerServerId = -1;
	this->serverId = -1;
	this->nodeCount = 0;
	this->remove = false;
	this->type = -1;
	this->task = -1;
	this->position = Vector3(0, 0, 0);
	this->rotations = Vector3(0, 0, 0);
}

Synchronization::Entity::Entity::~Entity()
{

}
#pragma endregion

void Synchronization::Entity::Add(Entity* _entity)
{
	if (entities != nullptr)
	{
		Entity** tempEnt = new Entity * [entityCount + 1];
		std::memcpy(tempEnt, entities, entityCount * sizeof(Entity*));
		delete[] entities;
		entities = tempEnt;
	}
	else
	{
		entities = new Entity * [1];
	}
	entities[entityCount] = _entity;
	entities[entityCount]->serverId = entityCountHistory;
	entityCount++;
	entityCountHistory++;
}


void Synchronization::Entity::Remove(int _serverId)
{

}

