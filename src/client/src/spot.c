#include "spots.h"

static bool		pixel_is_over_threshold(t_pixel pixel, int threshold) {
	if ((int)(pixel.g + pixel.r + pixel.b) > threshold)
		return (true);
	return (false);
}

static t_count_helper	recursive_count_island(int index, t_pixel *image, int depth) {
	int				target;
	t_count_helper	result;
	t_count_helper	recurs;
	depth++;

	g_pixel_buffer[index] = false;

	result.x = index % WIDTH;
	result.y = index / WIDTH;
	result.color.b = image[index].b;
	result.color.g = image[index].g;
	result.color.r = image[index].r;
	result.size = 1;
	if (depth > MAX_DEPTH)
	{
		fprintf(stderr, "coucou\n");
		DEPTH_REACH = true;
		return result;
	}
	else if (DEPTH_REACH == true)
		return result;

	//up
	if (index > WIDTH) {
		target = index - WIDTH;
		if (g_pixel_buffer[target]) {
			recurs = recursive_count_island(target, image, depth);
			result.x += recurs.x;
			result.y += recurs.y;
			result.color.b += recurs.color.b;
			result.color.g += recurs.color.g;
			result.color.r += recurs.color.r;
			result.size += recurs.size;
		}
	}
	//right
	if ((index + 1) % WIDTH) {
		target = index + 1;
		if (g_pixel_buffer[target]) {
			recurs = recursive_count_island(target, image, depth);
			result.x += recurs.x;
			result.y += recurs.y;
			result.color.b += recurs.color.b;
			result.color.g += recurs.color.g;
			result.color.r += recurs.color.r;
			result.size += recurs.size;
		}
	}
	//left
	if (index % WIDTH) {
		target = index - 1;
		if (g_pixel_buffer[target]) {
			recurs = recursive_count_island(target, image, depth);
			result.x += recurs.x;
			result.y += recurs.y;
			result.color.b += recurs.color.b;
			result.color.g += recurs.color.g;
			result.color.r += recurs.color.r;
			result.size += recurs.size;
		}
	}
	//bottom
	if ((index / WIDTH) + 1 < HEIGHT) {
		target = index + WIDTH;
		if (g_pixel_buffer[target]) {
			recurs = recursive_count_island(target, image, depth);
			result.x += recurs.x;
			result.y += recurs.y;
			result.color.b += recurs.color.b;
			result.color.g += recurs.color.g;
			result.color.r += recurs.color.r;
			result.size += recurs.size;
		}
	}
	return (result);
}

t_pos		count_island(int index, t_pixel *image) {
	t_count_helper	result = recursive_count_island(index, image, 0);
	t_pos			ret;

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
	int		size;
	t_count_helper	result;

	memset(&g_pixel_buffer, 0, (SIZETOTAL * sizeof(bool)));
	for (i = 0; i < SIZETOTAL; i++) {
		g_pixel_buffer[i] = pixel_is_over_threshold(image[i], THRESHOLD);
	}
	for (i = 0; i < SIZETOTAL; i++) {
		if (return_spot_array_size >= MAX_SPOTS)
			break ;
		if (DEPTH_REACH == true)
			break ;
		if (g_pixel_buffer[i])
		{
			result = recursive_count_island(i, image, 0);
			if (result.size > 10 && result.size < 240)
			{
				spot_array[return_spot_array_size].x = result.x / result.size;
				spot_array[return_spot_array_size].y = result.y / result.size;
				spot_array[return_spot_array_size].color.b = result.color.b / result.size;
				spot_array[return_spot_array_size].color.g = result.color.g / result.size;
				spot_array[return_spot_array_size].color.r = result.color.r / result.size;
				return_spot_array_size++;
			}
		}
	}
	if (DEPTH_REACH == true) {
		DEPTH_REACH = false;
		THRESHOLD++;
		fprintf(stderr, "Too much pixels parsed, upping threshold to a more estrictive value :\n%i\n", THRESHOLD);
		return (0);
	}

	return (return_spot_array_size);
}
