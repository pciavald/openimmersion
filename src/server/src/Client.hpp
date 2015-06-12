#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <netinet/in.h>

typedef struct sockaddr_in		t_sockaddr_in;
typedef struct sockaddr			t_sockaddr;

class Client
{
	public:
		Client(int & fd, t_sockaddr_in & addr);
		~Client(void);
		Client(void);
		int				get_fd(void);

	private:
		int				_fd;
		t_sockaddr_in	_addr;

		Client &	operator=(const Client &);
		Client(const Client &);
};

#endif
