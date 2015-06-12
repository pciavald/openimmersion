#ifndef SPOTS_H
# define SPOTS_H

#include "openimmersion.h"

#define bool			char
#define true			1
#define false			0

#define SIZETOTAL		WIDTH * HEIGHT
#define THRESHOLD		350

typedef struct	s_pixel {
	uint8_t		b;
	uint8_t		g;
	uint8_t		r;
}				t_pixel;

typedef struct 	s_pos {
	t_pixel		color;
	int			x;
	int			y;
}				t_pos;

typedef struct	s_int_color {
	int			b;
	int			g;
	int			r;
}				t_int_color;

typedef struct	s_count_helper {
	t_int_color	color;
	int			x;
	int			y;
	int			size;
}				t_count_helper;

typedef struct	s_packet
{
	size_t		size;
	t_pos		data[256];
}				t_packet;

bool		g_pixel_buffer[SIZETOTAL];

#endif
