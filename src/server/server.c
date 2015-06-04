/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

typedef struct sockaddr_in		t_sockaddr_in;

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int		sockfd;
int		newsockfd;
int		fps = 0;

int		update_fps(void)
{
	static int		count = 0;
	static time_t	t1;
	time_t			t2;

	if (count == 0)
		time(&t1);
	time(&t2);
	count++;
	if (t2 > t1)
	{
		fps = count;
		count = 0;
		return (1);
	}
	return (0);
}

void	sig_handler(int val)
{
	switch (val)
	{
		case SIGPIPE:
			fprintf(stderr, "broken pipe\n");
			break ;
		case SIGINT:
			fprintf(stderr, "interrupt\n");
			break ;
	}
	close(newsockfd);
	close(sockfd);
	exit(1);
}

#define BUFF_SIZE	64*48+100

int main(int argc, char *argv[])
{
	int				portno;
	socklen_t		clilen;
	char			buffer[BUFF_SIZE];
	t_sockaddr_in	serv_addr;
	t_sockaddr_in	cli_addr;
	int				n;

	signal(SIGPIPE, sig_handler);
	signal(SIGINT, sig_handler);

	if (argc < 2)
	{
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");

	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR on binding");

	listen(sockfd,5);

	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0) 
		error("ERROR on accept");

	int		stat = 0;
	bzero(buffer, BUFF_SIZE);
	while ((n = read(newsockfd, buffer, BUFF_SIZE)) > 0)
	{
		stat = update_fps();
		if (n < 0) error("ERROR reading from socket");
		printf("received %i\n", n);
		if (stat) printf("%i\n", fps);
		bzero(buffer, BUFF_SIZE);
	}

	//n = write(newsockfd,"I got your message",18);
	//if (n < 0) error("ERROR writing to socket");
	close(newsockfd);
	close(sockfd);
	return 0; 
}
