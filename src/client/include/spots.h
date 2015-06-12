#ifndef SPOTS_H
# define SPOTS_H

#include "openimmersion.h"

#define bool			char
#define true			1
#define false			0

#define SIZETOTAL		WIDTH * HEIGHT

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
	int			b;
	int			g;
	int			r;
	int			x;
	int			y;
	int			size;
}				t_count_helper;

bool		g_pixel_buffer[SIZETOTAL];

#endif
