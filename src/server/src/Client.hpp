#ifndef CLIENT_HPP
# define CLIENT_HPP

class Client
{
	public:
		Client(void);
		~Client(void);

	private:
		int		_fd;

		Client &	operator=(const Client &);
		Client(const Client &);
};

#endif
