#ifndef SYNC_H
#define SYNC_H
#include <cstdint>
#include <shared_mutex>
#include <chrono>
#include "../Shared/Vector.h"
#include "Connections.h"
#include <nlohmann/json.hpp>

#define DEFAULT_NODE_RANGE 300.0f

typedef unsigned long Hash;

namespace Synchronization
{
	namespace Entity
	{
		class Entity
		{
		public:/* !!! IN CASE OF MODIFICATION, MODIFY NODES.CPP TOO !!! */
			std::shared_mutex mutex;
			int ownerServerId;/*person who has control*/
			int serverId;/*server local*/

			std::chrono::time_point<std::chrono::high_resolution_clock> lastUpdate;
			int elapsedTime;

			bool remove;

			Hash hash;
			int type;
			int task;
			Vector3 position;
			Vector3 rotation;

			nlohmann::json ToJson();
			void UpdateFromJson(std::string _entityString);
			void Update(void);

			Entity();
			~Entity();
		};

		void Add(Entity* _entity);
		void Remove(int serverId);
		void EntityDataUpdate(const char* _data);
	}


	namespace Node
	{
		class Node
		{
		public:
			std::shared_mutex mutex;
			int serverId;
			float range;
			Vector3 position;

			Entity::Entity* entities;/*Needs a copy of each*/
			unsigned int entityCount;

			void InsertEntity(Entity::Entity* _entity);
			void UpdateEntity(Entity::Entity* _entity, int _entId);
			std::string GetAsJsonString(void);
			void Refresh(void);
			Node();
			~Node();
		};

		void Add(int _serverId, Vector3 _position, Entity::Entity* _owner);
		void Remove(int _serverId);


		void Update(int _serverId, Vector3 _position);

		void TriggerCallback(int _serverId);

		void AskForRefresh(int _serverId);

		void EntityDistCheck(Entity::Entity* _entity);


	}
	void MainEvent(char** _args);

}


#endif