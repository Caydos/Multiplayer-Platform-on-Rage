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
	void Kick(int socketfd, const char* _reason);

	void SendData(int connectionId, char* _buffer);

	std::int64_t GetDiscordId(void);
	void SetDiscordId(std::int64_t _id);
}


#endif // !CONNECTIONS_H
