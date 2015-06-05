#include "openimmersion.h"
#include <errno.h>

void	sig_handler(int sig)
{
	switch (sig)
	{
		case SIGINT:
			fprintf(stderr, "interrupt\n");
			break ;
		case SIGPIPE:
			fprintf(stderr, "broken pipe\n");
			break ;
	}
	if (g_data.server > 2)
		close(g_data.server);
	stop_capture();
	fprintf(stderr, "successfully destroyed all components, leaving.\n");
	exit(0);
}

void	check(int status, const char * func, int line, char * msg)
{
	if (status == MMAL_SUCCESS || status ==  0)
		return ;
	fprintf(stderr, "error %i, %s:%i :\n%s\n", status, func, line, msg);
	if (errno)
		fprintf(stderr, "%s\n", strerror(errno));
	exit(status);
}

void	dump(uint8_t * data, uint32_t length)
{
	write(1, (const void *)data, (size_t)length);
	fprintf(stderr, "dumped\n");
}

void	update_fps(void)
{
	static int		count = 0;
	static time_t	t1;
	time_t			t2;

	if (count == 0)
		time(&t1);
	time(&t2);
	count++;
	if (t2 > t1)
	{
		g_data.fps = count;
		count = 0;
		fprintf(stderr, "%i\n", g_data.fps);
	}
}
