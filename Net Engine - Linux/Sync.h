#ifndef SYNC_H
#define SYNC_H
#include <cstdint>
#include <shared_mutex>
#include "../Shared/Vector.h"
#include "Connections.h"

#define DEFAULT_NODE_RANGE 300.0f
namespace Synchronization
{
	namespace Entity
	{
		class Entity
		{
		public:/* !!! IN CASE OF MODIFICATION, MODIFY NODES.CPP TOO !!! */
			std::shared_mutex shEntMtx;
			int ownerServerId;/*person who has control*/
			int serverId;/*server local*/
			int nodeCount;/*nb of nodes it's in*/
			Entity* nodesPresence[MAX_CONNECTIONS];
			bool remove;

			int type;
			int task;
			Vector3 position;
			Vector3 rotations;

			void NodeInsertion(void);

			Entity();
			~Entity();
		};

		void Add(Entity* _entity);
		void Remove(int serverId);
	}


	namespace Node
	{
		class Node
		{
		public:
			int serverId;
			float range;
			Vector3 position;

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

		void Add(int _serverId, Vector3 _position, Entity::Entity* _owner);
		void Remove(int _serverId);


		void SendUpdate(int _serverId, Vector3 _position);
		void AskForRefresh(int _serverId);

		void EntityDistCheck(Entity::Entity* _entity);

	}
	void MainEvent(char** _args);

}


#endif