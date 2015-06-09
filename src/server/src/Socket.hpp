#ifndef SOCKET_HPP
# define SOCKET_HPP

#include "server.hpp"
#include "Client.hpp"
#include <netinet/in.h>
#include <iostream>
#include <vector>

#define PORT		42000
#define MAX_PEERS	1

typedef struct sockaddr_in		t_sockaddr_in;
typedef struct sockaddr			t_sockaddr;

class Socket
{
	public:
		Socket(void);
		~Socket(void);

		int *	getClients(void);

	private:
		int				_fd;
		t_sockaddr_in	_addr;

		int				_fds	[MAX_PEERS];
		t_sockaddr_in	_addrs	[MAX_PEERS];

		Client			_left;
		Client			_right;

		Socket &	operator=(const Socket &);
		Socket(const Socket &);
};

#endif
