#include "Header.h"
#include "List.h"
#include "Events.h"
#include "Sync.h"


void fkt(char** _args)
{
	std::cout << "Function triggered" << std::endl;
}

void ListEvents(void)
{
	//RegisterServerEvent("PlayerConnection", &PlayerConnection);
	//RegisterServerEvent("Way_01", &Way_01);
	Events::Register("Damn", &fkt);
}