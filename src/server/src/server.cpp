#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

#include "Socket.hpp"
#include "Ros.hpp"

static void		get(t_packet * p, int * clients)
{
	int			i;
	int			n;
	int			acc = 0;
	char *		begin;
	char *		buff = NULL;

	while (i < MAX_PEERS)
	{
		buff = reinterpret_cast<char *>(&p[i]);
		n = read(clients[i], buff + acc, sizeof(t_packet));
		cerr << "recv " << n << endl;
		if (n < 0)
			throw (string("could not read from socket"));
		if (n == 0)
			throw (string("broken pipe"));
		if (n + acc < sizeof(t_packet))
		{
			acc += n;
			i++;
			continue ;
		}
		if (n + acc > sizeof(t_packet))
			throw (string("buffer overflow"));
		acc = 0;
	}
}

static void		dump(t_packet * packet)
{
	t_pos *		pos;

	cerr << "-- packet has " << packet->size << " entries" << endl;
	for (int i = 0; i < packet->size; i++)
	{
		pos = &packet->data[i];
		cerr << "x " << pos->x << " y " << pos->y << endl;
		cerr << "col #";
		cerr << pos->color.r;
		cerr << pos->color.g;
		cerr << pos->color.b << endl;
	}
}

static void		compute(geometry_msgs::Point * pose, t_packet * packets)
{
	/*
	 * triangulation algorithm happends here
	 * packets[MAX_PEERS] is an array of MAX_PEERS packets
	 * cf server.hpp or spots.h
	 */

	cerr << sizeof(size_t) + 4 * sizeof(t_pos) << endl;
	// test
	for (int i = 0; i < MAX_PEERS; i++)
	{
		//cerr << endl << "peer " << i << endl;
		//dump(&packets[i]);
	}
	pose->x = 1;
	pose->y = 2;
	pose->z = 3;
}

static void		clean(t_packet * p)
{
	for (int i = 0; i < MAX_PEERS; i++)
		bzero(&p[i], sizeof (t_packet));
}

int		main(int argc, char ** argv)
{
	try
	{
		init(argc, argv);
		Socket		s;
		Ros			ros;
		t_packet	packets[MAX_PEERS];

		geometry_msgs::Point	pose;

		while (42)
		{
			clean(packets);
			get(packets, s.getClients());
			compute(&pose, packets);
			ros.send(pose);
		}
	}
	catch (string & s) {
		cerr << s << endl;
	}
	return (0);
}
