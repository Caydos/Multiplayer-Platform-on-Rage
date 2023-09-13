#ifndef CONNECTIONS_H
#define CONNECTIONS_H


#define MAX_CONNECTIONS 15


namespace Connections
{
	void Detect(int socketfd);
	void Accept(int socketfd);
	void Reject(int socketfd);

	void Remove(int connectionId);
	void Lost(int socketfd);

	void SendData(int connectionId, char* _buffer);
}


#endif // !CONNECTIONS_H
