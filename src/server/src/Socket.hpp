#ifndef SOCKET_HPP
# define SOCKET_HPP

#include "server.hpp"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

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
		int				_clients[MAX_PEERS];

		Socket &	operator=(const Socket &);
		Socket(const Socket &);
};

#endif
