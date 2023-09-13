
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
		valueString << typeid(_arg).raw_name() << _arg; // Potential issue 1
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
		buffer[bufferSize - 2] = 'µ'; // Null-terminate the string
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
	evtSv.buffer[evtSv.bufferSize - 2] = 'µ';

	((evtSv.SerializeArg(_args)), ...);
	evtSv.buffer[evtSv.bufferSize - 2] = '§';
	printf("%s\n", evtSv.buffer);
	send(clientSocket, evtSv.buffer, strlen(evtSv.buffer), 0);
	delete[] targetString;
	delete[] evtSv.buffer;
	evtSv.buffer = nullptr;
	evtSv.bufferSize = 0;
};

int main()
{
	InitSocket();
	Connect();
	while (true)
	{
		TriggerServerEvent("Damn", "dong", 147.5f, true, "On the hello fluxys", "Baguette", 30360, "Zoinks");
		Sleep(1000);
	}

	return 0;
}


