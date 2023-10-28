#include "Header.h"
#include "Sync.h"

using namespace Synchronization;

std::shared_mutex sh_ent_mtx;
Entity::Entity** entities;
int entityCount = 0;

#pragma region Constructors / Destructors
Entity::Entity::Entity()
{
	this->serverId = -1;
	this->needsCreation = false;
	this->needsDeletion = false;
	this->type = -1;
	this->task = -1;
	this->position = Vector3(0,0,0);
	this->rotations = Vector3(0, 0, 0);
}

Entity::Entity::~Entity()
{

}
#pragma endregion



void Entity::Entity::Update()
{
	std::shared_lock<std::shared_mutex> lock(sh_ent_mtx);
	Node::EntityDistCheck(this);
}

Entity::Entity* Entity::Add(Entity _entity)
{
	std::unique_lock<std::shared_mutex> lock(sh_ent_mtx);

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
	entity->serverId = entityCount;

	entities[entityCount] = entity;
	entityCount++;

	return entities[entityCount];
}


void Entity::Remove(Entity* _entity)
{
	std::unique_lock<std::shared_mutex> lock(sh_ent_mtx);

}


