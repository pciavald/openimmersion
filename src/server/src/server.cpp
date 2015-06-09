#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "Socket.hpp"
#include <strings.h>

#define PORT		42000
#define BUFF_SIZE	640*480 * 3

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
	exit(1);
}

void	init(void)
{
	signal(SIGPIPE, sig_handler);
	signal(SIGINT, sig_handler);
}

void	dump(Socket & s, int c)
{
	int		n;
	char	buff[BUFF_SIZE];

	bzero(buff, BUFF_SIZE);
	while ((n = read(s.getClients()[0], buff, BUFF_SIZE)) > 0)
	{
		if (n < 0) throw (string("could not read from socket"));
		write(1, buff, BUFF_SIZE);
	}
}

int		main(void)
{
	Socket		s;
	init();
	dump(s, 1);
	return (0);
}
