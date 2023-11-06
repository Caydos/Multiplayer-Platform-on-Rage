#include "Header.h"
#include "Sync.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

using namespace Synchronization;

auto global_start = std::chrono::high_resolution_clock::now();

std::shared_mutex shEntMtx;
Entity::Entity** entities;
int entityCount = 0;
unsigned int entityCountHistory = 0;

#pragma region Constructors / Destructors

Entity::Entity::Entity()
{
	this->ownerServerId = -1;
	this->serverId = -1;
	this->remove = false;

	auto now = std::chrono::high_resolution_clock::now();
	this->lastUpdate = now;
	this->elapsedTime = 0;

	this->type = -1;
	this->task = -1;
	this->position = Vector3(0, 0, 0);
	this->rotation = Vector3(0, 0, 0);
}

Entity::Entity::~Entity()
{

}
#pragma endregion

json Synchronization::Entity::Entity::ToJson()
{
	json j;
	if (this->type == 1)
	{
		json position_json = {
			{"x", this->position.x},
			{"y", this->position.y},
			{"z", this->position.z}
		};

		json rotation_json = {
			{"x", this->rotation.x},
			{"y", this->rotation.y},
			{"z", this->rotation.z}
		};
		j = json{
			{"ownerServerId", this->ownerServerId},
			{"serverId", this->serverId},
			{"elapsedTime", this->elapsedTime},
			{"remove", this->remove},
			{"hash", this->hash},
			{"type", this->type},
			{"task", this->task},
			{"position", position_json},
			{"rotation", rotation_json}
		};
	}
	return j;
}

void Synchronization::Entity::Entity::UpdateFromJson(std::string _entityString)
{
	std::shared_lock<std::shared_mutex> lock(shEntMtx);
	//std::cout << "Updating from json :" << _entityString << std::endl;
	json j_entity_parsed = json::parse(_entityString);
	{
		std::unique_lock<std::shared_mutex> entLock(this->mutex);

		this->remove = j_entity_parsed.at("remove").get<bool>();
		this->hash = j_entity_parsed.at("hash").get<unsigned long>();
		this->type = j_entity_parsed.at("type").get<int>();
		this->task = j_entity_parsed.at("task").get<int>();

		this->position.x = j_entity_parsed.at("position").at("x").get<float>();
		this->position.y = j_entity_parsed.at("position").at("y").get<float>();
		this->position.z = j_entity_parsed.at("position").at("z").get<float>();

		this->rotation.x = j_entity_parsed.at("rotation").at("x").get<float>();
		this->rotation.y = j_entity_parsed.at("rotation").at("y").get<float>();
		this->rotation.z = j_entity_parsed.at("rotation").at("z").get<float>();

		if (this->remove)
		{// Remove entity everywhere
			std::cout << "Entity with id : " << this->serverId << " needs to be removed" << std::endl;
		}
	}
	this->Update();
}

void Synchronization::Entity::Entity::Update(void)
{
	std::shared_lock<std::shared_mutex> lock(shEntMtx);
	{
		std::unique_lock<std::shared_mutex> EntLock(this->mutex);
		auto now = std::chrono::high_resolution_clock::now();
		this->lastUpdate = now;
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - this->lastUpdate);

		this->elapsedTime = duration.count();
	}
	std::shared_lock<std::shared_mutex> EntLock(this->mutex);
	Node::EntityDistCheck(this);
}

void Entity::Add(Entity* _entity)
{
	std::unique_lock<std::shared_mutex> lock(shEntMtx);
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
	std::cout << "Creating a new entity with id : " << entities[entityCount-1]->serverId << std::endl;
}


void Entity::Remove(int _serverId)
{
	std::unique_lock<std::shared_mutex> lock(shEntMtx);
	bool foundTarget = false;
	for (unsigned int entityId = 0; entityId < entityCount; entityId++)
	{
		if (entities[entityId]->serverId == _serverId)
		{
			std::unique_lock<std::shared_mutex> EntLock(entities[entityId]->mutex);
			delete entities[entityId];
			entities[entityId] = nullptr;
			foundTarget = true;
		}
		// Shift the remaining elements up to fill the gap
		if (foundTarget && entityId < entityCount - 1)
		{
			std::unique_lock<std::shared_mutex> EntLock(entities[entityId]->mutex);
			entities[entityId]->mutex;//here
			entities[entityId] = entities[entityId + 1];
		}
	}
	if (foundTarget)
	{
		entityCount--; // Decrement count only if an entity was removed
		Entity** tempEnt = nullptr;
		if (entityCount > 0)
		{
			tempEnt = new Entity * [entityCount];
			std::memcpy(tempEnt, entities, entityCount * sizeof(Entity*));
		}
		delete[] entities;
		entities = tempEnt;
	}
}

void Synchronization::Entity::EntityDataUpdate(const char* _data)
{
	std::shared_lock<std::shared_mutex> lock(shEntMtx);
	json j_entities = json::parse(_data);
	for (const auto& j_entity : j_entities)
	{
		int serverId = j_entity.at("serverId").get<int>();
		if (serverId != -1)/* Is registered server Side ? */
		{
			for (unsigned int entityId = 0; entityId < entityCount; entityId++)
			{
				if (entities[entityId]->serverId == serverId)
				{
					std::string entityString = j_entity.dump();
					entities[entityId]->UpdateFromJson(entityString);
				}
			}
		}
		else
		{
			Entity* ent = new Entity;
			//ent->serverId = j_entity.at("serverId").get<int>();
			ent->ownerServerId = j_entity.at("ownerServerId").get<int>();
			// ... Do this for all Entity fields
			ent->remove = j_entity.at("remove").get<bool>();
			ent->hash = j_entity.at("hash").get<unsigned long>();
			ent->type = j_entity.at("type").get<int>();
			ent->task = j_entity.at("task").get<int>();

			ent->position.x = j_entity.at("position").at("x").get<float>();
			ent->position.y = j_entity.at("position").at("y").get<float>();
			ent->position.z = j_entity.at("position").at("z").get<float>();

			ent->rotation.x = j_entity.at("rotation").at("x").get<float>();
			ent->rotation.y = j_entity.at("rotation").at("y").get<float>();
			ent->rotation.z = j_entity.at("rotation").at("z").get<float>();
			Add(ent);
		}
	}
}
