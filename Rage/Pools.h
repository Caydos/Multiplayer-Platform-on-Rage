#ifndef POOLS_H
#define POOLS_H
#include "PedData.h"



namespace SyncPools
{
	class Peds
	{
	private:
		unsigned int count = 0;
		Pedestrian* peds = nullptr;
	public:
		void Add(Ped);
		void Remove(Ped);
		void Sync();
	};

}






#endif // !POOLS_H
