#include "openimmersion.h"
#include <fcntl.h>

void	non_blocking_read(int fd)
{
	char	buffer[1024];
	int		flags = fcntl(fd, F_GETFL, 0);
	size_t	size = 1024;

	bzero(buffer, size);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	if (read(fd, buffer, size) != -1)
	{
		fprintf(stderr, "%s\n", buffer);
	}
}

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
		non_blocking_read(1);
	}
	return (0);
}
