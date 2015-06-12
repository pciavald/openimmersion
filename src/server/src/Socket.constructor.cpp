#include "Socket.hpp"

static void		open_socket(int & fd)
{
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 2)
		throw (string("could not open socket"));
}

static void		set_address(t_sockaddr_in & addr)
{
	addr.sin_family			= AF_INET;
	addr.sin_addr.s_addr	= INADDR_ANY;
	addr.sin_port			= htons(PORT);
}

static void		bind_port(int & fd, t_sockaddr_in * addr, size_t size)
{
	t_sockaddr *	sockaddr;

	sockaddr = reinterpret_cast<t_sockaddr *>(addr);
	if (bind(fd, sockaddr, size) < 0)
		throw (string("could not bind socket"));
}

static void		wait_for_peers(int & sock, int * clients)
{
	int				client;
	int				fd;
	t_sockaddr *	addr;
	t_sockaddr_in	addr_in;
	socklen_t		addr_len;
	
	cerr << "waiting for peers to connect..." << endl;
	addr = reinterpret_cast<t_sockaddr *>(&addr_in);
	addr_len = sizeof (addr_in);
	while (client < MAX_PEERS)
	{
		//fd = accept(sock, addr, &addr_len);
		fd = accept(sock, NULL, NULL);
		if (fd < 2)
			throw (string("error connecting with client #%i", client));
		clients[client++] = fd;
		cerr << "received a connection, remaining ";
		cerr << MAX_PEERS - client << endl;
	}
}

Socket::Socket(void)
{
	t_sockaddr_in	addr;

	try
	{
		open_socket(_fd);
		set_address(addr);
		bind_port(_fd, &addr, sizeof (addr));
		listen(_fd, MAX_PEERS);
		wait_for_peers(_fd, (int *)&_clients);
	}
	catch (string & s) {
		cerr << s << endl;
	}
}
