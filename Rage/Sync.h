#ifndef SYNC_H
#define SYNC_H
#include "../Shared/Vector.h"

#define SYNC_TIME 3000


namespace Synchronization
{
	namespace Entity
	{
		class Entity
		{
		public:/* !!! IN CASE OF MODIFICATION, MODIFY NODES.CPP TOO !!! */
			int ownerServerId;/*person who has control*/
			int serverId;/*server local*/
			int nodeCount;/*nb of nodes it's in*/
			bool remove;

			int type;
			int task;
			Vector3 position;
			Vector3 rotations;

			Entity();
			~Entity();
		};

		void Add(Entity* _entity);
		void Remove(int serverId);
	}
	void Init(void);
	void Loop(void);
	void NodeDataEvent(char** _args);


	void PlayerLanding(char** _args);
	void PedSyncEvents(void);
}


#endif