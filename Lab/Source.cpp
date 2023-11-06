
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>


#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <psapi.h>
#include <stdbool.h>
#include <cstdint>
#include <typeinfo>
#include <iostream>
#include <sstream>
#include <cstring>


#define SERVER_PORT 55000
#define SERVER_IP "162.19.67.68"
const char END_CHARACTER = '§';
const char SEPARATOR_CHARACTER = 'µ';
SOCKET clientSocket;

bool InitSocket(void)
{
	WSADATA wsdata;
	if (WSAStartup(MAKEWORD(2, 2), &wsdata) != 0)
	{
		return false;
	}
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	return true;
}

bool Connect(void)
{
	SOCKADDR_IN serverAddress;
	ZeroMemory(&serverAddress, sizeof(serverAddress));

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(SERVER_PORT);
	serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP);

	if (connect(clientSocket, (SOCKADDR*)(&serverAddress), sizeof(serverAddress)) != 0)
	{
		printf("Failed to connect\n");
		return false;
	}
	printf("Connected\n");
	return true;
}



class EventSv
{
public:
	char* buffer = nullptr;
	unsigned int bufferSize = 0;

	template<typename Arg>
	void SerializeArg(Arg _arg)
	{
		std::stringstream valueString;
		valueString /*<< typeid(_arg).raw_name()*/ << _arg; // Potential issue 1
		std::string argStr = valueString.str();

		int argLength = argStr.length();

		int oldBufferSize = bufferSize - 1;
		bufferSize += (argLength + 1); // +1 for the null-terminator

		char* newBuffer = new char[bufferSize + 1];

		if (buffer != nullptr) {
			std::memcpy(newBuffer, buffer, oldBufferSize);
			delete[] buffer;
		}

		buffer = newBuffer;
		//std::cout << argStr << ' ' << oldBufferSize << std::endl;

		for (int i = 0; i < argLength; i++)
		{
			buffer[oldBufferSize + i] = argStr[i];
		}
		buffer[bufferSize - 2] = SEPARATOR_CHARACTER; // Null-terminate the string
		buffer[bufferSize - 1] = '\0'; // Null-terminate the string
		//printf("%s\n", buffer);
	}
};

template<typename Name, class...Arg>
void TriggerServerEvent(Name _name, Arg..._args)
{
	EventSv evtSv;
	int nameLength = strlen(_name);

	char* targetString = new char[nameLength];
	sprintf(targetString, "%s", _name);
	evtSv.bufferSize = strlen(targetString) + 2;
	evtSv.buffer = new char[evtSv.bufferSize];
	strcpy(evtSv.buffer, targetString);
	evtSv.buffer[evtSv.bufferSize - 2] = SEPARATOR_CHARACTER;

	((evtSv.SerializeArg(_args)), ...);
	evtSv.buffer[evtSv.bufferSize - 2] = END_CHARACTER;
	evtSv.buffer[evtSv.bufferSize - 1] = '\0';
	std::cout << evtSv.buffer << std::endl;
	send(clientSocket, evtSv.buffer, strlen(evtSv.buffer) + 1, 0);
	delete[] targetString;
	delete[] evtSv.buffer;
	evtSv.buffer = nullptr;
	evtSv.bufferSize = 0;
};

#include "../Shared/Encryption.h"
#include "../Shared/Reading.h"
#include <nlohmann/json.hpp>


using json = nlohmann::json;

struct Vector3
{
	float x;
	float y;
	float z;
};

class Encrypt
{
public:
	int id;
	char* name;
	Vector3 vec;
};

const int ARRAY_SIZE = 1024;

int main()
{
	InitSocket();
	Connect();
	Sleep(1000);
	TriggerServerEvent("PlayerConnection", 112, 1885233650, -424.21f, 1204.52f, 325.75f);
	while (true)
	{
		//TriggerServerEvent("Damn", "dong", 149.56f, true, "Ta soeur la coquillete", "Zebi", 52, "Cordialement");
		//TriggerServerEvent("Synchronization::MainEvent", 56,47,69, 
		//	R"([[["ownerServerId",-1],["serverId", -1]]])");
		Sleep(3000);
	}
	closesocket(clientSocket);
	return 0;
}

