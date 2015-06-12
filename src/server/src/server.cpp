#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "Socket.hpp"
#include "Ros.hpp"
#include <strings.h>

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
