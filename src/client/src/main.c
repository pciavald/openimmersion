#include "openimmersion.h"

int		main(void)
{
	int		i = 1;

	init();
	init_client(SERVER_NAME, SERVER_PORT);
	g_data.start = 1;
	while (42)
	{
		if (i)
		{
			wait_for_server(g_data.server, (t_sockaddr *)&g_data.internet_socket);
			i = 0;
		}
		if (g_data.start)
		{
			g_data.start = 0;
			init_camera();
			init_preview();
			start_capture();
		}
	}
	return (0);
}
