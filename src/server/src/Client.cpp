#include "Client.hpp"

Client::Client(void)
{
}

int		Client::get_fd(void)
{
	return (_fd);
}

Client::Client(int & fd, t_sockaddr_in & addr) :
	_fd(fd),
	_addr(addr)
{
}

Client::~Client(void)
{
}
