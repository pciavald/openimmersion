#include "openimmersion.h"

int		main(void)
{
	init();
	g_data.start = 1;
	while (42)
	{
		if (g_data.connected == false)
		{
			init_client(SERVER_NAME, SERVER_PORT);
			wait_for_server(g_data.server, (t_sockaddr *)&g_data.internet_socket);
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
