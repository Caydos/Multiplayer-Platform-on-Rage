#include "pch.h"
#include "PSync.h"
#include "PCreation.h"

void PedSyncEvents(void)
{
	Events::Register("PedSync::Creation", &PedSync::Creation);
	Events::Register("Way_01", &PedSync::Main);
}







