#include "openimmersion.h"
#include "spots.h"
#include <errno.h>
#include <fcntl.h>

void	sig_handler(int sig)
{
	switch (sig)
	{
		case SIGINT:
			fprintf(stderr, "interrupt\n");
			break ;
		case SIGPIPE:
			fprintf(stderr, "broken pipe\n");
			g_data.connected = false;
			close(g_data.server);
			return ;
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

static bool		pixel_is_over_threshold(t_pixel pixel, int threshold) {
	if ((int)(pixel.g + pixel.r + pixel.b) > threshold)
		return (true);
	return (false);
}

void	dump(uint8_t * data, uint32_t length, char * name)
{
	int			i;
	int			fd1;
	int			fd2;
	mode_t		mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;
	t_pixel *	img = (t_pixel *)data;
	t_pixel		empty = {0};
	char		tname[strlen(name)];

	name[strlen(name) - 1] = '\0';
	strncpy(tname, name, sizeof(tname));
	tname[strlen(tname)] = 't';
	fd1 = open(name, O_CREAT | O_WRONLY, mode);
	fd2 = open(tname, O_CREAT | O_WRONLY, mode);
	write(fd1, (const void *)data, (size_t)length);
	for (i = 0; i < SIZETOTAL; i++)
	{
		if (pixel_is_over_threshold(img[i], THRESHOLD))
			write(fd2, (const void *)&(img[i]), sizeof(t_pixel));
		else
			write(fd2, (const void *)&empty, sizeof(t_pixel));
	}
	close(fd1);
	close(fd2);
	fprintf(stderr, "wrote file %s\n", DUMP);
	bzero(DUMP, sizeof(DUMP));
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
		//fprintf(stderr, "%i\n", g_data.fps);
	}
}
