#include "openimmersion.h"

int		main(void)
{
	int				connected = 0;
	t_sockaddr_in	sin;

	init();
	sin = init_client(SERVER_NAME, SERVER_PORT);
	while (42)
	{
		if (connected == 0)
		{
			wait_for_server((t_sockaddr *)&sin);
			connected = 1;
		}
	}

	/*
	start_capture();
	while (42)
		;
	*/
	return (0);
}
