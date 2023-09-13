#include "pch.h"
#include "PSync.h"
#include "PCreation.h"

void PedSyncEvents(void)
{
	RegisterClientEvent("PedSync::Creation", &PedSync::Creation);
	RegisterClientEvent("Way_01", &PedSync::Main);
}







