#ifndef SERVER_HPP
# define SERVER_HPP

using namespace std;
#include <iostream>

#define PORT		42000
#define MAX_PEERS	1
#define MAX_SPOTS	256
#define BUFF_SIZE	1024

void	init(int argc, char ** argv);

#include <stdint.h>

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

#endif
