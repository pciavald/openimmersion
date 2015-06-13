#include <signal.h>
#include "Ros.hpp"

void	sig_handler(int val)
{
	switch (val)
	{
		case SIGPIPE:
			cerr << "broken pipe" << endl;
			break ;
		case SIGINT:
			cerr << "interrupt" << endl;
			break ;
	}
	exit(1);
}

void	init(int argc, char ** argv)
{
	signal(SIGPIPE, sig_handler);
	signal(SIGINT, sig_handler);
	ros::init(argc, argv, "server", ros::init_options::NoSigintHandler);
}
