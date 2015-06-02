#include "openimmersion.h"

void	check(int status, const char * func, int line, char * msg)
{
	if (status == MMAL_SUCCESS)
		return ;
	fprintf(stderr, "error %i, %s:%i :\n%s\n", status, func, line, msg);
	exit(status);
}

void	dump(uint8_t * data, uint32_t length)
{
	write(1, (const void *)data, (size_t)length);
	fprintf(stderr, "dumped\n");
}

int		mesure_time(int frame)
{
	static struct timespec	t1;
	struct timespec 		t2;

	if (frame == 0)
		clock_gettime(CLOCK_MONOTONIC, &t1);
	clock_gettime(CLOCK_MONOTONIC, &t2);
	return (t2.tv_sec - t1.tv_sec);
}

void	print_time(int frame, int d, int length)
{
	(void)length;
	if (frame % 30 == 0)
		fprintf(stderr, "%d fps\n", frame / (d + 1));
}
