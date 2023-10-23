#include "pch.h"
#include "PSync.h"
#include "PCreation.h"

void PedSyncEvents(void)
{
	Events::Register("PedSync::Creation", &PedSync::Creation);
	Events::Register("SyncMe", &PedSync::Main);
}







