#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "Socket.hpp"
#include "Ros.hpp"
#include <strings.h>

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

void	init(int argc, char ** argv)
{
	signal(SIGPIPE, sig_handler);
	signal(SIGINT, sig_handler);
	ros::init(argc, argv, "server");
}

void	dump(Socket * s, int c)
{
	int		i = 0;
	int		n;
	char	buff[BUFF_SIZE];

	bzero(buff, BUFF_SIZE);
	while ((n = read(s->getClients()[0], buff, BUFF_SIZE)) > 0)
	{
		if (n < 0) throw (string("could not read from socket"));
		if (i++ == 300)
			write(1, buff, BUFF_SIZE);
		bzero(buff, BUFF_SIZE);
	}
}

static void		get(char * buff, Socket * sock)
{
	int			n;
	int			acc = 0;
	char *		begin;

	for (int i = 0; i < MAX_PEERS; i++)
	{
		n = read(sock->getClients()[i], buff + acc, BUFF_SIZE);
		if (n < 0)
			throw (string("could not read from socket"));
		if (n == 0)
			throw (string("broken pipe"));
		if (n + acc < BUFF_SIZE)
		{
			acc += n;
			continue ;
		}
		if (n + acc > BUFF_SIZE)
			throw (string("buffer overflow"));
	}
}

static void		compute(geometry_msgs::Point * pose, char * buff)
{
	/*
	 * triangulation algorithm happends here
	 */

	// test
	pose->x = 1;
	pose->y = 2;
	pose->z = 3;
}

static void		clean(char * buff)
{
	//for (int i = 0; i < MAX_PEERS; i++)
		bzero(buff, BUFF_SIZE);
}

int		main(int argc, char ** argv)
{
	try
	{
		init(argc, argv);
		Socket		s;
		Ros			ros;
		char		buff[BUFF_SIZE];

		geometry_msgs::Point	pose;

		while (42)
		{
			clean((char *)buff);
			get((char *)buff, &s);
			compute(&pose, (char *)buff);
			ros.send(pose);
			cerr << "lol" << endl;
		}
	}
	catch (string & s) {
		cerr << s << endl;
	}
	return (0);
}
