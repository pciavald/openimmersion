#include "Socket.hpp"

int *	Socket::getClients(void) {return (_clients);}

Socket::~Socket(void)
{
	cerr << "closing connections" << endl;
	for (int i = 0; i < MAX_PEERS; i++)
		close(_clients[i]);
	close(_fd);
}
