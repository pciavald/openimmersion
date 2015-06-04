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

	fprintf(stderr, "starting video\n");
	g_status = mmal_port_parameter_set_boolean(video, MMAL_PARAMETER_CAPTURE, 1);
	check(g_status, __func__, __LINE__, "starting capture");
	fprintf(stderr, "waiting for stable fps... ");
	while (g_data.fps != FPS)
		;
	fprintf(stderr, "done : %i fps.\n", FPS);
}

void	stop_capture(void)
{
	MMAL_PORT_T *	video = g_data.camera_video_port;

	g_stop = 1;
	g_status = mmal_port_parameter_set_boolean(video, MMAL_PARAMETER_CAPTURE, 0);
	check(g_status, __func__, __LINE__, "starting capture");
	//g_status = mmal_port_disable(g_data.camera_video_port);
	//check(g_status, __func__, __LINE__, "disable camera video port");
	//mmal_port_pool_destroy(
	//		g_data.camera_video_port,
	//		g_data.camera_video_port_pool);
	g_status = mmal_port_disable(g_data.preview_input_port);
	check(g_status, __func__, __LINE__, "disable preview input port");
	mmal_port_pool_destroy(
			g_data.preview_input_port,
			g_data.preview_input_port_pool);
	g_status = mmal_component_disable(g_data.preview);
	check(g_status, __func__, __LINE__, "disable preview");
	g_status = mmal_component_disable(g_data.camera);
	check(g_status, __func__, __LINE__, "disable camera");
	g_status = mmal_component_destroy(g_data.preview);
	check(g_status, __func__, __LINE__, "destroy preview");
	//g_status = mmal_component_destroy(g_data.camera);
	//check(g_status, __func__, __LINE__, "destroy camera");
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
