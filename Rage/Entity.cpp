#include "Sync.h"
#include <cstring>
#include "Scripting.h"
#include <iostream>

using json = nlohmann::json;

Synchronization::Entity::Entity** entities;
int entityCount = 0;

#pragma region Constructors / Destructors

void Synchronization::Entity::Entity::Create(void)
{
	std::cout << "Creating new entity" << std::endl;
	if (this->type == 1)
	{
		STREAMING::REQUEST_MODEL(this->hash);

		while (!STREAMING::HAS_MODEL_LOADED(this->hash))
		{
			std::cout << "Loading model" << std::endl;
			Fibers::Suspend(100);
		}

		this->localId = PED::CREATE_PED(1, this->hash, this->position, 0.0f, true, true);
		ENTITY::SET_ENTITY_AS_MISSION_ENTITY(this->localId, true, false);
		TASK::TASK_SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(this->localId, true);

		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(this->hash);
		std::cout << "Ped created succefully" << std::endl;
	}
}

void Synchronization::Entity::Entity::UpdateLocal(void)
{
	if (this->ownerServerId == playerId)
	{
		NativeVector3 pos = ENTITY::GET_ENTITY_COORDS(this->localId, true);
		this->position = Vector3(pos.x, pos.y, pos.z);
		if (this->type == 1)
		{//Get ped specific data

		}
	}
	else
	{
		if (this->type == 1)
		{
			NativeVector3 entPos = ENTITY::GET_ENTITY_COORDS(this->localId, true);
			if (Vdist(this->position, Vector3(entPos.x, entPos.y, entPos.z)) > 1.0f)
			{
				TASK::TASK_PED_SLIDE_TO_COORD(this->localId, this->position, 0.0f, 90.0f);
			}
		}
	}
}

nlohmann::json Synchronization::Entity::Entity::ToJson()
{
	json j;
	if (this->type == 1)
	{
		nlohmann::json position_json = {
			{"x", this->position.x},
			{"y", this->position.y},
			{"z", this->position.z}
		};

		nlohmann::json rotation_json = {
			{"x", this->rotation.x},
			{"y", this->rotation.y},
			{"z", this->rotation.z}
		};
		j = nlohmann::json{
			{"ownerServerId", this->ownerServerId},
			{"serverId", this->serverId},
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
	json j_entity_parsed = json::parse(_entityString);
	{			
		this->serverId = j_entity_parsed.at("serverId").get<int>();
		this->ownerServerId = j_entity_parsed.at("ownerServerId").get<int>();
		this->remove = j_entity_parsed.at("remove").get<bool>();
		this->elapsedTime = j_entity_parsed.at("elapsedTime").get<int>();

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
	this->UpdateLocal();
}

Synchronization::Entity::Entity::Entity()
{
	this->ownerServerId = -1;
	this->serverId = -1;
	this->remove = false;
	this->elapsedTime = 0;

	this->localId = 0;
	this->hash = 0;
	this->type = -1;
	this->task = -1;
	this->position = Vector3(0, 0, 0);
	this->rotation = Vector3(0, 0, 0);
}

Synchronization::Entity::Entity::~Entity()
{

}
#pragma endregion


void Synchronization::Entity::Add(Entity* _entity, bool _create)
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
	if (_create)
	{
		entities[entityCount]->Create();
	}
	entityCount++;
}


void Synchronization::Entity::Remove(int _serverId)
{
	bool foundTarget = false;
	for (unsigned int entityId = 0; entityId < entityCount; entityId++)
	{
		if (entities[entityId]->serverId == _serverId)
		{
			delete entities[entityId];
			entities[entityId] = nullptr;
			foundTarget = true;
		}
		// Shift the remaining elements up to fill the gap
		if (foundTarget && entityId < entityCount - 1)
		{
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

void Synchronization::Entity::SetOwnerServerId(int _localId, int _ownerServerId)
{
	for (unsigned int entityId = 0; entityId < entityCount; entityId++)
	{
		if (entities[entityId]->localId == _localId)
		{
			entities[entityId]->ownerServerId = _ownerServerId;
		}
	}
}

void Synchronization::Entity::SetServerId(int _localId, int _serverId)
{
	for (unsigned int entityId = 0; entityId < entityCount; entityId++)
	{
		if (entities[entityId]->localId == _localId)
		{
			entities[entityId]->serverId = _serverId;
		}
	}
}

std::string Synchronization::Entity::GetAsJsonString(void)
{
	std::string str;
	if (entityCount)
	{
		json jsonArray = json::array();
		for (unsigned int entityId = 0; entityId < entityCount; entityId++)
		{
			if (entities[entityId]->ownerServerId == playerId)
			{
				entities[entityId]->UpdateLocal();
				jsonArray.push_back(entities[entityId]->ToJson());
			}
		}
		str = jsonArray.dump();
	}
	return str;
}



void Synchronization::Entity::UpdateFromJson(const char* _data)
{
	json j_entities = json::parse(_data);
	for (const auto& j_entity : j_entities)
	{
		bool exists = false;
		int serverId = j_entity.at("serverId").get<int>();
		for (unsigned int entityId = 0; entityId < entityCount; entityId++)
		{
			if (entities[entityId]->serverId == serverId)
			{
				exists = true;
				std::string entityString = j_entity.dump();
				entities[entityId]->UpdateFromJson(entityString);
			}
		}
		if (!exists)
		{
			std::cout << "Don't exists for now" << std::endl;
			Entity* ent = new Entity;
			ent->ownerServerId = j_entity.at("ownerServerId").get<int>();
			ent->serverId = j_entity.at("serverId").get<int>();
			ent->remove = j_entity.at("remove").get<bool>();
			ent->elapsedTime = j_entity.at("elapsedTime").get<int>();
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
			Add(ent, true);
		}
	}
}
