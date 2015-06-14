#include "openimmersion.h"
#include "spots.h"
#include <sys/socket.h>

static void		send_buffer(t_packet * packet)
{
	if (g_data.connected)
	{
		g_status = send(g_data.server, &(packet->header), sizeof(t_hdr), 0);
		//check(g_status < 0 ? -1 : 0, __func__, __LINE__, "sending header");
		g_status = send(g_data.server, packet, packet->header.size, 0);
		//check(g_status < 0 ? -1 : 0, __func__, __LINE__, "sending data");
	}
}

static size_t	weight(t_packet * packet)
{
	return (sizeof(t_hdr) + packet->header.elems * sizeof(t_pos));
}

static void		use_buffer(uint8_t * buffer, uint32_t buffer_length, int frame)
{
	t_packet	pack;

	// dump a BGR888 frame
	if (frame == -1)
		dump(buffer, buffer_length);

	bzero(&pack, sizeof (pack));
	pack.header.elems = detect_spots(pack.data, buffer);
	pack.header.size = weight(&pack);

	send_buffer(&pack);
}

static void		send_new_buffer_to_port(MMAL_POOL_T * pool, MMAL_PORT_T * port)
{
	MMAL_BUFFER_HEADER_T *	new_buffer;

	new_buffer = mmal_queue_get(pool->queue);
	if (new_buffer && port->is_enabled)
		g_status = mmal_port_send_buffer(port, new_buffer);
}

void	video_buffer_callback(MMAL_PORT_T * port, MMAL_BUFFER_HEADER_T * buffer)
{
	static int		frame			= 0;
	MMAL_POOL_T *	pool			= (MMAL_POOL_T *)port->userdata;
	MMAL_POOL_T *	preview_pool	= g_data.preview_input_port_pool;
	MMAL_PORT_T *	preview_port	= g_data.preview_input_port;

	if (g_data.start == 1)
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
