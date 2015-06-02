#include "openimmersion.h"

void	create_component(const char * name, MMAL_COMPONENT_T ** c, char * msg)
{
	g_status = mmal_component_create(name, c);
	check(g_status, __func__, __LINE__, msg);
}

void	enable_component(MMAL_COMPONENT_T * component, char * msg)
{
	g_status = mmal_component_enable(component);
	check(g_status, __func__, __LINE__, msg);
}

void	start_capture(void)
{
	MMAL_PORT_T *	video = g_data.camera_video_port;

	g_status = mmal_port_parameter_set_boolean(video, MMAL_PARAMETER_CAPTURE, 1);
	check(g_status, __func__, __LINE__, "starting capture");
}

void	create_pool_on_port(
		MMAL_POOL_T **		pool,
		MMAL_PORT_T *		port,
		MMAL_PORT_BH_CB_T	callback,
		char *				msg)
{
	unsigned int			headers	= port->buffer_num;
	uint32_t				size	= port->buffer_size;

	*pool = (MMAL_POOL_T *)mmal_port_pool_create(port, headers, size);
	port->userdata = (struct MMAL_PORT_USERDATA_T *)(*pool);
	g_status = mmal_port_enable(port, callback);
	check(g_status, __func__, __LINE__, msg);
}
