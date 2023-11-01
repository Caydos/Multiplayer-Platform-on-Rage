#include "pch.h"
#include "Events.h"
#include "Logger.h"
#include "Threads.h"

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
	std::thread serverPackagesThread(Events::Listener, clientSocket);
	serverPackagesThread.detach();
	printf("Connected\n");
	return true;
}

void CloseConnection(void)
{
	closesocket(clientSocket);
}