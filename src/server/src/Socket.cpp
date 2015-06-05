#include "Socket.hpp"
#include <sys/socket.h>

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

static void		bind_port(int fd, t_sockaddr_in * addr, size_t size)
{
	t_sockaddr *	sockaddr;

	sockaddr = reinterpret_cast<t_sockaddr *>(addr);
	if (bind(fd, sockaddr, size) < 0)
		throw (string("could not bind socket"));
}

static void		wait_for_peers(int fd, int * fds, t_sockaddr_in * addrs)
{
	int				client;
	t_sockaddr *	addr;
	socklen_t		addr_len;
	
	cout << "waiting for peers to connect..." << endl;
	while (client < MAX_PEERS)
	{
		addr = reinterpret_cast<t_sockaddr *>(&addrs[client]);
		addr_len = sizeof (addrs[client]);
		fds[client] = accept(fd, addr, &addr_len);
		if (fds[client] < 2)
			throw (string("error connecting with client #%i", client));
		client++;
		cout << "received a connection, remaining ";
		cout << MAX_PEERS - client << endl;
	}
}

static void		set_clients(int * fds, t_sockaddr_in * addrs)
{
}

Socket::Socket(void)
{
	int				fds		[MAX_PEERS];
	t_sockaddr_in	addrs	[MAX_PEERS];

	open_socket(_fd);
	set_address(_addr);
	bind_port(_fd, &_addr, sizeof (_addr));
	listen(_fd, MAX_PEERS);
	wait_for_peers(_fd, fds, addrs);
	set_clients(fds, addrs);
}
