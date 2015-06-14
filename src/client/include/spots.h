#ifndef SPOTS_H
# define SPOTS_H

#include "openimmersion.h"

#define MAX_SPOTS		256

typedef struct	s_pixel {
	uint8_t		b;
	uint8_t		g;
	uint8_t		r;
}				t_pixel;

typedef struct 	s_pos {
	t_pixel		color;
	uint32_t	x;
	uint32_t	y;
}				t_pos;

typedef struct	s_hdr
{
	uint16_t	elems;
	uint16_t	size;
}				t_hdr;

typedef struct	s_packet
{
	t_hdr		header;
	t_pos		data[MAX_SPOTS];
}				t_packet;

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

bool		g_pixel_buffer[SIZETOTAL];

#endif
