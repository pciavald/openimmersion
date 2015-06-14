#include "openimmersion.h"
#include "spots.h"
#include <sys/socket.h>


static void		send_new_buffer_to_port(MMAL_POOL_T * pool, MMAL_PORT_T * port)
{
	MMAL_BUFFER_HEADER_T *	new_buffer;

	new_buffer = mmal_queue_get(pool->queue);
	if (new_buffer && port->is_enabled)
		g_status = mmal_port_send_buffer(port, new_buffer);
}

static void		use_buffer(void *buffer) {
	t_pixel	*image = (t_pixel *)buffer;
}

void	calibrate(MMAL_PORT_T * port, MMAL_BUFFER_HEADER_T * buffer)
{
	MMAL_POOL_T *	pool			= (MMAL_POOL_T *)port->userdata;
	MMAL_POOL_T *	preview_pool	= g_data.preview_input_port_pool;
	MMAL_PORT_T *	preview_port	= g_data.preview_input_port;

	if (g_data.start == 1)
	{
		//use_buffer(buffer);
		mmal_buffer_header_release(buffer);
		send_new_buffer_to_port(pool, port);
		send_new_buffer_to_port(preview_pool, preview_port);
	}
}
