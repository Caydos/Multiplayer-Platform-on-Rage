#ifndef SYNC_H
#define SYNC_H
#include "Framework.h"

#define SYNC_TIME 800

namespace Synchronization
{
	void Loop(void);
	void NodeCallback(char** _args);

	void PedSyncEvents(void);
}


#endif // !SYNC_H