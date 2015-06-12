#include "openimmersion.h"
#include "spots.h"
#include <sys/socket.h>

static void		send_buffer(t_packet * packet)
{
	g_status = send(g_data.server, &(packet->header), sizeof(t_hdr), 0);
	check(g_status < 0 ? -1 : 0, __func__, __LINE__, "sending header");
	g_status = send(g_data.server, packet, packet->header.size, 0);
	check(g_status < 0 ? -1 : 0, __func__, __LINE__, "sending data");
}

static size_t	weight(t_packet * packet)
{
	return (sizeof(t_hdr) + packet->header.elems * sizeof(t_pos));
}

static size_t	test(void * data, void * buffer)
{
	size_t		spots		= 0;
	t_pos *		position	= (t_pos *)data;

	(void)data;
	spots = 4;
	position[0].color.b = 42;
	position[0].color.g = 42;
	position[0].color.r = 42;
	position[0].x = 42;
	position[0].y = 42;
	position[1].color.b = 42;
	position[1].color.g = 42;
	position[1].color.r = 42;
	position[1].x = 42;
	position[1].y = 42;
	position[2].color.b = 42;
	position[2].color.g = 42;
	position[2].color.r = 42;
	position[2].x = 42;
	position[2].y = 42;
	position[3].color.b = 42;
	position[3].color.g = 42;
	position[3].color.r = 42;
	position[3].x = 42;
	position[3].y = 42;

	return (spots);
}

static void		use_buffer(uint8_t * buffer, uint32_t buffer_length, int frame)
{
	t_packet	pack;

	// dump a BGR888 frame
	if (frame == -1)
		dump(buffer, buffer_length);

	bzero(&pack, sizeof (pack));
	//pack.size = detect_spots(pack.data, buffer);
	pack.header.elems = test(pack.data, buffer);
	pack.header.size = weight(&pack);

	send_buffer(&pack);
}

static void		send_new_buffer_to_port(MMAL_POOL_T * pool, MMAL_PORT_T * port)
{
	MMAL_BUFFER_HEADER_T *	new_buffer;

	new_buffer = mmal_queue_get(pool->queue);
	if (new_buffer && port->is_enabled)
		g_status = mmal_port_send_buffer(port, new_buffer);
	else
		check(-1, __func__, __LINE__, "Unable to get new buffer");
	check(g_status, __func__, __LINE__, "Unable to send new buffer");
}

void	video_buffer_callback(MMAL_PORT_T * port, MMAL_BUFFER_HEADER_T * buffer)
{
	static int		frame			= 0;
	MMAL_POOL_T *	pool			= (MMAL_POOL_T *)port->userdata;
	MMAL_POOL_T *	preview_pool	= g_data.preview_input_port_pool;
	MMAL_PORT_T *	preview_port	= g_data.preview_input_port;

	if (g_stop == 0)
	{
		update_fps();
		frame++;
		use_buffer(buffer->data, buffer->length, frame);
		mmal_buffer_header_release(buffer);
		send_new_buffer_to_port(pool, port);
		send_new_buffer_to_port(preview_pool, preview_port);
	}
}

void	preview_buffer_callback(MMAL_PORT_T * port, MMAL_BUFFER_HEADER_T * buffer)
{
	(void)port;
	mmal_buffer_header_release(buffer);
}
