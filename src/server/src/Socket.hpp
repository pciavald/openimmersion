#ifndef SOCKET_HPP
# define SOCKET_HPP

#include "server.hpp"
//#include "Client.hpp"
#include <netinet/in.h>
#include <iostream>
#include <vector>
#include <unistd.h>

#define PORT		42000

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
		//t_sockaddr_in	_addr;
		//int				_nbClients;
		//vector<Client>	_clients;

		//void			_addClient(int & fd, t_sockaddr_in & addr);

		//int				_fds	[MAX_PEERS];
		//t_sockaddr_in	_addrs	[MAX_PEERS];

		Socket &	operator=(const Socket &);
		Socket(const Socket &);
};

#endif
