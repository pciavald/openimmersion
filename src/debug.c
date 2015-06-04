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

void	print_time(int frame)
{
	static struct timespec	t1;
	struct timespec 		t2;
	int						d;

	if (frame % 30 == 0)
	{
		if (frame == 0)
			clock_gettime(CLOCK_MONOTONIC, &t1);
		clock_gettime(CLOCK_MONOTONIC, &t2);
		d = t2.tv_sec - t1.tv_sec;
		fprintf(stderr, "%d fps\n", frame / (d + 1));
	}
}
