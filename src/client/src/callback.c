#include "openimmersion.h"
#include <sys/socket.h>

static void		send_buffer(void * data, size_t length)
{
	g_status = send(g_data.server, data, length, 0);
	check(g_status < 0 ? -1 : 0, __func__, __LINE__, "sending data");
}

static void		use_buffer(uint8_t * buffer, uint32_t buffer_length, int frame)
{
	size_t		data_length = 1024;
	char		data[data_length];

	// dump a YUV frame
	if (frame == -1)
		dump(buffer, buffer_length);

	memset(data, data_length, '\0');
	/*
	 * process buffer here
	 * output data in &data
	 * set length of data_length
	 */

	// test
	size_t i;
	for (i = 0; i < data_length; i++)
		data[i] = i % 256;

	send_buffer(data, data_length);
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
