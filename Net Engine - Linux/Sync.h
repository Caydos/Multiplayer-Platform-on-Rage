#ifndef SYNC_H
#define SYNC_H
#include <cstdint>
#include "Vector.h"

void PlayerConnection(char** _args);
void SyncMe(char** _args);

#define DEFAULT_NODE_RANGE 300.0f
namespace Synchronization
{
	class Entity
	{
	public:
		int type;
		int task;
		Vector3 position;
		Vector3 rotations;

		void Update();
		Entity();
		~Entity();
	};

	class Node
	{
	public:
		std::int64_t discordId;
		float range;
		Vector3 position;

		Entity** entities;
		unsigned int entityCount;

		void InsertEntity(Entity* _entity);
		void Refresh(void);
		Node();
		~Node();
	};

	void AddEntity(Entity _entity);
	void AddNode(std::int64_t _discordId, Vector3 _position);
}


#endif