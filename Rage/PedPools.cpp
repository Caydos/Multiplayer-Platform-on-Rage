#include "pch.h"
#include "Pools.h"






void SyncPools::Peds::Add(Ped _id)
{
	count++;
	SyncPed* table = new SyncPed[count];
	delete[] peds;
	peds = table;
	peds[count - 1].Identifier = _id;
}

void SyncPools::Peds::Remove(Ped _id)
{
	if (count == 0)
	{
		return;
	}
	bool found = false;
	for (unsigned int i = 0; i < count; i++)
	{
		if (_id == peds[i].Identifier)
		{
			found = true;
		}
		if (found)
		{
			peds[i] = peds[i + 1];
		}
	}
	count--;
}



void SyncPools::Peds::Sync()
{

}
