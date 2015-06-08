#include "openimmersion.h"

void	set_preview_port(MMAL_PORT_T * camera_preview_port)
{
	MMAL_ES_FORMAT_T *		format;

	format = camera_preview_port->format;
	format->encoding				= MMAL_ENCODING_OPAQUE;
	format->encoding_variant		= MMAL_ENCODING_I420;
	format->es->video.width			= WIDTH;
	format->es->video.height		= HEIGHT;
	format->es->video.crop.x		= 0;
	format->es->video.crop.y		= 0;
	format->es->video.crop.width	= WIDTH;
	format->es->video.crop.height	= HEIGHT;
	g_status = mmal_port_format_commit(camera_preview_port);
	check(g_status, __func__, __LINE__, "could not commit preview format");
}

void	set_video_port(MMAL_PORT_T * camera_video_port)
{
	MMAL_ES_FORMAT_T *		format;

	format = camera_video_port->format;
	format->encoding					= MMAL_ENCODING_I420;
	format->encoding_variant			= MMAL_ENCODING_I420;
	format->es->video.width				= WIDTH;
	format->es->video.height			= HEIGHT;
	format->es->video.crop.x			= 0;
	format->es->video.crop.y			= 0;
	format->es->video.crop.width		= WIDTH;
	format->es->video.crop.height		= HEIGHT;
	format->es->video.frame_rate.num	= FPS;
	format->es->video.frame_rate.den	= 1;
	camera_video_port->buffer_size = camera_video_port->buffer_size_recommended;
	camera_video_port->buffer_num = 2;
	g_status = mmal_port_format_commit(camera_video_port);
	check(g_status, __func__, __LINE__, "could not commit video format");
}

void	set_preview_input_port(void)
{
	MMAL_ES_FORMAT_T *		format;

	format = g_data.preview_input_port->format;
	format->encoding					= MMAL_ENCODING_I420;
	format->encoding_variant			= MMAL_ENCODING_I420;
	format->es->video.width				= WIDTH;
	format->es->video.height			= HEIGHT;
	format->es->video.crop.x			= 0;
	format->es->video.crop.y			= 0;
	format->es->video.crop.width		= WIDTH;
	format->es->video.crop.height		= HEIGHT;
	format->es->video.frame_rate.num	= FPS;
	format->es->video.frame_rate.den	= 1;
	g_data.preview_input_port->buffer_size = g_data.camera_video_port->buffer_size_recommended;
	g_data.preview_input_port->buffer_num = 4;
	g_status = mmal_port_format_commit(g_data.preview_input_port);
	check(g_status, __func__, __LINE__, "could not commit preview input format");
}
