#include "openimmersion.h"
#include <fcntl.h>

#define SHUTTER_S	"shutter:"

void	non_blocking_read(int fd)
{
	char	buffer[1024];
	int		flags = fcntl(fd, F_GETFL, 0);
	size_t	size = 1024;

	bzero(buffer, size);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	if (read(fd, buffer, size) != -1)
	{
		if (!strcmp(buffer, "start\n"))
		{
			fprintf(stderr, "START\n");
			start_capture();
		}
		else if (!strcmp(buffer, "stop\n"))
		{
			fprintf(stderr, "STOP\n");
			stop_capture();
		}
		else if (!strncmp(buffer, SHUTTER_S, sizeof(SHUTTER_S) -1))
		{
			int		speed;

			fprintf(stderr, "set shutter speed to %s",
					buffer + sizeof(SHUTTER_S) - 1);
			speed = atoi(buffer + sizeof(SHUTTER_S) - 1);
			if (speed > 0)
				SHUTTER_SPEED = speed;
			stop_capture();
			start_capture();
		}
		else
			fprintf(stderr, "unknown command: %s\n", buffer);
	}
}

int		main(void)
{
	init();
	g_data.start = 0;
	while (42)
	{
		if (g_data.connected == false)
		{
			init_client(SERVER_NAME, SERVER_PORT);
			wait_for_server(g_data.server, (t_sockaddr *)&g_data.internet_socket);
		}
		non_blocking_read(1);
	}
	return (0);
}
