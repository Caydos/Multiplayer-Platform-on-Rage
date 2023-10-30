#include "Header.h"
#include "List.h"
#include "Events.h"
#include "Reading.h"
#include "SyncEvents.h"
#include "Sync.h"

void fkt(char** _args)
{
	char* firstArg = ToString(_args[0]);
	float val = ToFloat(_args[1]);
	bool tr = ToBool(_args[2]);
	char* firstArg2 = ToString(_args[3]);
	char* firstArg3 = ToString(_args[4]);
	int teger = ToInt(_args[5]);
	char* firstArg4 = ToString(_args[6]);


	std::cout << firstArg << std::endl;
	std::cout << val << std::endl;
	std::cout << tr << std::endl;
	std::cout << firstArg2 << std::endl;
	std::cout << firstArg3 << std::endl;
	std::cout << teger << std::endl;
	std::cout << firstArg4 << std::endl;
}


void ListEvents(void)
{
	Events::Register("PlayerConnection", &PlayerConnection);
	Events::Register("Synchronization::MainEvent", &Synchronization::MainEvent);


	/*Test one*/
	Events::Register("Damn", &fkt);
}