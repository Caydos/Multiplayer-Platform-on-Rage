#ifndef SYNC_H
#define SYNC_H
#include <cstdint>
#include "Vector.h"


#define DEFAULT_NODE_RANGE 300.0f
namespace Synchronization
{
	namespace Entity
	{
		class Entity
		{
		public:
			int serverId;
			int ownerId;
			int entId;/*owner local*/
			bool needsCreation;
			bool needsDeletion;

			int type;
			int task;
			Vector3 position;
			Vector3 rotations;

			void Update();
			Entity();
			~Entity();
		};

		Entity* Add(Entity _entity);
		void Remove(Entity* _entity);
	}


	namespace Node
	{
		class Node
		{
		public:
			bool upToDate;
			std::int64_t discordId;
			float range;
			Vector3 position;
			Entity::Entity* owner;

			Entity::Entity* entities;/*Needs a copy of each*/
			unsigned int entityCount;
			Node** nodes;
			unsigned int nodeCount;

			void InsertEntity(Entity::Entity* _entity);
			void UpdateEntity(Entity::Entity* _entity, int _entId);
			void InsertNode(Node* _node);
			void Refresh(void);
			Node();
			~Node();
		};

		Node* Add(std::int64_t _discordId, Vector3 _position, Entity::Entity* _owner);
		void Remove(std::uint64_t _discordId);
		void SendUpdate(std::uint64_t _discordId, Vector3 _position);

		void EntityDistCheck(Entity::Entity* _entity);
	}
}


#endif