#include "openimmersion.h"

static void		use_buffer(uint8_t * data, uint32_t length, int frame)
{
	if (frame == -1)
		dump(data, length);
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
		update_fps(frame);
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
