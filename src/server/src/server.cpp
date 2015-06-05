#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT	42000

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
	//close(newsockfd);
	//close(sockfd);
	exit(1);
}

void	init(void)
{
	signal(SIGPIPE, sig_handler);
	signal(SIGINT, sig_handler);
}

int		main(void)
{
	init();
	return (0);
}
