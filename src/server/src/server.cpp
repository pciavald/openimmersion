#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

#include "Socket.hpp"
#include "Ros.hpp"

static void		dump(t_packet * packet)
{
	t_pos *		pos;

	cerr << "-- packet has " << packet->header.elems << " entries and weights ";
	cerr << packet->header.size << endl;
	for (int i = 0; i < packet->header.elems; i++)
	{
		pos = &packet->data[i];
		cerr << "x " << pos->x << " y " << pos->y << endl;
		cerr << "col #";
		cerr << pos->color.r;
		cerr << pos->color.g;
		cerr << pos->color.b << endl;
	}
}

static void		get(t_packet * packets, int * clients)
{
	int			n;
	int			acc;
	int			i = 0;
	t_hdr		header;
	char *		buff = NULL;

	while (i < MAX_PEERS)
	{
		bzero(&header, sizeof(t_hdr));
		n = read(clients[i], &header, sizeof(t_hdr));
		if (n < 1)
			throw (string("could not read size from socket"));
		buff = reinterpret_cast<char *>(&(packets[i]));
		acc = n = 0;
		while (acc + n < header.size)
		{
			n = read(clients[i], buff + acc, header.size);
			if (n < 1)
				throw (string("could not read data from socket"));
			acc += n;
			n = 0;
		}
		acc = 0;
		i++;
	}
}

static void		compute(geometry_msgs::Point * pose, t_packet * packets)
{
	/*
	 * triangulation algorithm happends here
	 * packets[MAX_PEERS] is an array of MAX_PEERS packets
	 * cf server.hpp or spots.h
	 */

	// test
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
