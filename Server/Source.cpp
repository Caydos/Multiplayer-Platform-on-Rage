#include "Header.h"
#include "Events.h"

int sockfd;
int playerCount;
Client playerList[MAX_CLIENTS];

int main(void)
{
	playerCount = 0;
	CreateEvents();

	printf("Started\n");
	int portno;
	struct sockaddr_in serv_addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		printf("ERROR opening socket");
	}
	bzero((char*)&serv_addr, sizeof(serv_addr));
	portno = atoi("55000");
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("ERROR on binding");
	}
	socklen_t clilen;
	struct sockaddr_in cli_addr;

	listen(sockfd, 5);
	clilen = sizeof(cli_addr);
	int clientId = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
	if (clientId < 0)
	{
		printf("Failed to accept Player Connection");
	}
	else
	{
		ConnectPlayer(clientId);
	}

	std::thread ptid(ConnectionListener);
	ptid.join();

	close(sockfd);
	printf("Server Closing\n\n");
	return 0;
	
}