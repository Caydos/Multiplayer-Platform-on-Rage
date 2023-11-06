#ifndef SYNC_H
#define SYNC_H
#include <string>
#include "../Shared/Vector.h"
#include "Types.h"
#include <nlohmann/json.hpp>

#define SYNC_TIME 800


namespace Synchronization
{
	namespace Entity
	{
		class Entity
		{
		public:/* !!! IN CASE OF MODIFICATION, MODIFY NODES.CPP TOO !!! */
			int ownerServerId;/*person who has control*/
			int serverId;/*server local*/
			bool remove;
			int elapsedTime;

			int localId;
			Hash hash;
			int type;/*0=Nothing;1=Ped;2=Vehicle;3=Object;*/
			int task;
			Vector3 position;
			Vector3 rotation;

			void Create(void);
			void UpdateLocal(void);
			void UpdateFromJson(std::string _entityString);

			nlohmann::json ToJson();
			Entity();
			~Entity();
		};

		void Add(Entity* _entity, bool _create = false);
		void Remove(int serverId);
		void SetOwnerServerId(int _localId, int _ownerServerId);
		void SetServerId(int _localId, int _serverId);
		std::string GetAsJsonString(void);
		void UpdateFromJson(const char* _data);
	}
	void Init(void);
	void Loop(void);
	void NodeDataEvent(char** _args);


	void PlayerLanding(char** _args);
	void PedSyncEvents(void);
}


#endif