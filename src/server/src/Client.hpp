#ifndef CLIENT_HPP
# define CLIENT_HPP

class Client
{
	public:
		Client(void);
		~Client(void);

	private:
		Client &	operator=(const Client &);
		Client(const Client &);
};

#endif
