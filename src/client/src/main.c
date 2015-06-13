#include "openimmersion.h"




int		main(void)
{
	t_sockaddr_in	sin;
	int	i = 1;

	init();
	sin = init_client(SERVER_NAME, SERVER_PORT);
	while (42)
	{
		if (!COMPUTE_CONNECTED)
			wait_for_server((t_sockaddr *)&sin);
		// read g_data.receive
		if (i)
		{
			i = 0;
			start_capture();
		}
	}

	/*
	start_capture();
	while (42)
		;
	*/
	return (0);
}
