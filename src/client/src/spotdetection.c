#define SIZETOTAL		307200

typedef struct 	s_pos {
	t_pixel		color;
	int			x;
	int			y;
}				t_pos;

typedef struct	s_pixel {
	uint8_t		g;
	uint8_t		b;
	uint8_t		r;
}				t_pixel;

typedef struct	s_int_color {
	int			g;
	int			b;
	int			r;
}				t_int_color;

typedef struct	s_count_helper {
	int	g;
	int	r;
	int	b;
	int	x;
	int	y;
	int	size;
}				t_count_helper;

bool		g_pixel_buffer[SIZETOTAL];

static bool		pixel_is_over_threshold(t_pixel pixel, int threshold) {
	if ((int)(pixel.g + pixel.r + pixel.b) > threshold)
		return (true);
	return (false);
}

static t_count_helper	recursive_count_island(int index, t_pixel *image) {
	int	target;
	t_count_helper	result;
	t_count_helper	recurs;

	g_pixel_buffer[index] = false;

	result.x = index % WIDTH;
	result.y = index / WIDTH;
	result.g = image[index].g;
	result.r = image[index].r;
	result.b = image[index].b;

	//up
	if (index > WIDTH) {
		target = index - WIDTH;
		if (g_pixel_buffer[target]) {
			recurs = recursive_count_island(target, image);
			result.x += recurs.x;
			result.y += recurs.y;
			result.color.g += recurs.color.g;
			result.color.b += recurs.color.b;
			result.color.r += recurs.color.r;
		}
	}
	//right
	if ((index + 1) % WIDTH) {
		target = index + 1;
		if (g_pixel_buffer[target]) {
			recurs = recursive_count_island(target, image);
			result.x += recurs.x;
			result.y += recurs.y;
			result.color.g += recurs.color.g;
			result.color.b += recurs.color.b;
			result.color.r += recurs.color.r;
		}
	}
	//left
	if ((index % WIDTH) {
		target = index - 1;
		if (g_pixel_buffer[target]) {
			recurs = recursive_count_island(target, image);
			result.x += recurs.x;
			result.y += recurs.y;
			result.color.g += recurs.color.g;
			result.color.b += recurs.color.b;
			result.color.r += recurs.color.r;
		}
	}
	//bottom
	if ((index / WIDTH) + 1 < HEIGHT) {
		target = index + WIDTH;
		if (g_pixel_buffer[target]) {
			recurs = recursive_count_island(target, image);
			result.x += recurs.x;
			result.y += recurs.y;
			result.color.g += recurs.color.g;
			result.color.b += recurs.color.b;
			result.color.r += recurs.color.r;
		}
	}
	return (result);
}

t_pos		count_island(int index, t_pixel *image) {
	t_count_helper	result = recursive_count_island(index, image);
	t_pos			ret;

	ret.x = result.x / result.size;
	ret.y = result.y / result.size;
	ret.color.g = result.g / result.size;
	ret.color.b = result.b / result.size;
	ret.color.r = result.r / result.size;
	return (ret);
}

size_t		detect_spots(void *data, void *buffer) {
	size_t	return_spot_array_size = 0;
	t_pos	*spot_array = (t_pos *)data;
	t_pixel	*image = (t_pixel *)buffer;

	//memset(&g_pixel_buffer, 0, (SIZETOTAL * sizeof(bool)));
	for (int i = 0; i < SIZETOTAL; i++) {
		g_pixel_buffer[i] = pixel_is_over_threshold(image[i], threshold));
	for (int i = 0; i < SIZETOTAL; i++) {
		if g_pixel_buffer[i]
			spot_array[return_spot_array_size++] = count_island(i, image);
	}
	return (return_spot_array_size);
}