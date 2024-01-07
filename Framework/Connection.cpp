#include "Network.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>

static bool initialized = false;

struct Connection
{
	SOCKET socket;
	SOCKADDR_IN address;
	std::string ip;
	unsigned int port;
	std::thread eventListener;
};
std::vector<Connection*> connections;

bool Network::Connection::Create(const char* _address, unsigned int _port)
{
	::Connection* connection = new ::Connection;
	if (!initialized)
	{
		WSADATA wsdata;
		if (WSAStartup(MAKEWORD(2, 2), &wsdata) != 0)
		{
			Logger::Write("Socket Initialization fail.", "\n");
			return false;
		}
		connection->socket = socket(AF_INET, SOCK_STREAM, 0);
		initialized = true;
	}
	ZeroMemory(&connection->address, sizeof(connection->address));

	connection->address.sin_family = AF_INET;
	connection->address.sin_port = htons(_port);
	connection->address.sin_addr.s_addr = inet_addr(_address);

	connection->ip = _address;
	connection->port = _port;

	int cd = connect(connection->socket, (SOCKADDR*)(&connection->address), sizeof(connection->address));
	if (cd != 0)
	{
		Logger::Write("Failed to connect to socket at address : ", connection->ip, " - ", connection->port, " Code : ", cd, "\n");
		delete connection;
		return false;
	}
	Logger::Write("Connection established at address : ", connection->ip, " - ", connection->port, "\n");
	connection->eventListener = std::thread(&Events::Listener, connection->socket);
	connection->eventListener.detach();

	connections.push_back(connection);
	return true;
}

void Network::Connection::Close(const char* _address, unsigned int _port)
{
	if (!initialized)
	{
		WSADATA wsdata;
		if (WSAStartup(MAKEWORD(2, 2), &wsdata) != 0)
		{
			Logger::GetFile() << "Socket Initialization fail." << std::endl;
			return;
		}
		initialized = true;
	}
	for (size_t i = 0; i < connections.size(); i++)
	{
		if (connections[i]->ip == _address && connections[i]->port == _port)
		{
			closesocket(connections[i]->socket);
			delete connections[i];
			connections.erase(connections.begin() + i);
			return;
		}
	}
	Logger::Write("Failed to connect to close socket at address : ", _address, " - ", _port, "\n");
}

void Network::Connection::Send(const char* _buffer, unsigned int _connectionId)
{
	send(connections.at(_connectionId)->socket, _buffer, strlen(_buffer) + 1, 0);
}

void Network::Connection::Shutdown(void)
{
	for (size_t i = 0; i < connections.size(); i++)
	{
		Close(connections[i]->ip.c_str(), connections[i]->port);
	}
}
