#include "Header.h"
#include "Connections.h"
#include "List.h"

#define SERVER_PORT "55000"
int socketfd;

int main(void)
{
	printf("Started\n");
	ListEvents();
	int portno;
	struct sockaddr_in serv_addr;

	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd < 0)
	{
		printf("ERROR opening socket");
	}
	bzero((char*)&serv_addr, sizeof(serv_addr));
	portno = atoi(SERVER_PORT);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(socketfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("ERROR on binding");
	}

	Connections::Detect(socketfd);

	close(socketfd);
	printf("Server Closing\n\n");
	return 0;

}