#include "spots.h"

static bool		pixel_is_over_threshold(t_pixel pixel, int threshold) {
	if ((int)(pixel.g + pixel.r + pixel.b) > threshold)
		return (true);
	return (false);
}

static t_count_helper	recursive_count_island(int index, t_pixel *image) {
	int				target;
	t_count_helper	result;
	t_count_helper	recurs;

	fprintf(stderr, "%i\n", index);
	g_pixel_buffer[index] = false;

	result.x = index % WIDTH;
	result.y = index / WIDTH;
	result.color.b = image[index].b;
	result.color.g = image[index].g;
	result.color.r = image[index].r;
	result.size = 1;

	//up
	if (index > WIDTH) {
		target = index - WIDTH;
		if (g_pixel_buffer[target]) {
			fprintf(stderr, "up\n");
			recurs = recursive_count_island(target, image);
			result.x += recurs.x;
			result.y += recurs.y;
			result.color.b += recurs.color.b;
			result.color.g += recurs.color.g;
			result.color.r += recurs.color.r;
		}
		result.size += recurs.size;
	}
	//right
	if ((index + 1) % WIDTH) {
		target = index + 1;
		if (g_pixel_buffer[target]) {
			fprintf(stderr, "right\n");
			recurs = recursive_count_island(target, image);
			result.x += recurs.x;
			result.y += recurs.y;
			result.color.b += recurs.color.b;
			result.color.g += recurs.color.g;
			result.color.r += recurs.color.r;
		}
		result.size += recurs.size;
	}
	//left
	if (index % WIDTH) {
		target = index - 1;
		if (g_pixel_buffer[target]) {
			fprintf(stderr, "left\n");
			recurs = recursive_count_island(target, image);
			result.x += recurs.x;
			result.y += recurs.y;
			result.color.b += recurs.color.b;
			result.color.g += recurs.color.g;
			result.color.r += recurs.color.r;
		}
		result.size += recurs.size;
	}
	//bottom
	if ((index / WIDTH) + 1 < HEIGHT) {
		target = index + WIDTH;
		if (g_pixel_buffer[target]) {
			fprintf(stderr, "bot\n");
			recurs = recursive_count_island(target, image);
			result.x += recurs.x;
			result.y += recurs.y;
			result.color.b += recurs.color.b;
			result.color.g += recurs.color.g;
			result.color.r += recurs.color.r;
		}
		result.size += recurs.size;
	}
	return (result);
}

t_pos		count_island(int index, t_pixel *image) {
	t_count_helper	result = recursive_count_island(index, image);
	t_pos			ret;

	check((result.size == 0), __func__, __LINE__, "division by 0");
	ret.x = result.x / result.size;
	ret.y = result.y / result.size;
	ret.color.b = result.color.b / result.size;
	ret.color.g = result.color.g / result.size;
	ret.color.r = result.color.r / result.size;
	return (ret);
}

size_t		detect_spots(void *data, void *buffer) {
	size_t	return_spot_array_size = 0;
	t_pos	*spot_array = (t_pos *)data;
	t_pixel	*image = (t_pixel *)buffer;
	int		i;

	//memset(&g_pixel_buffer, 0, (SIZETOTAL * sizeof(bool)));
	for (i = 0; i < SIZETOTAL; i++) {
		g_pixel_buffer[i] = pixel_is_over_threshold(image[i], THRESHOLD);
	}
	for (i = 0; i < SIZETOTAL; i++) {
		if (g_pixel_buffer[i])
		{
			fprintf(stderr, "reccall\n");
			spot_array[return_spot_array_size++] = count_island(i, image);
		}
	}
	return (return_spot_array_size);
}
