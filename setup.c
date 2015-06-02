#include "openimmersion.h"

void	configure_camera(MMAL_COMPONENT_T * camera)
{
	MMAL_PARAMETER_CAMERA_CONFIG_T cam_config = {
		{ MMAL_PARAMETER_CAMERA_CONFIG, sizeof (cam_config)},
		.max_stills_w							= WIDTH,
		.max_stills_h							= HEIGHT,
		.stills_yuv422							= 0,
		.one_shot_stills						= 1,
		.max_preview_video_w					= WIDTH,
		.max_preview_video_h					= HEIGHT,
		.num_preview_video_frames				= 3,
		.stills_capture_circular_buffer_height	= 0,
		.fast_preview_resume					= 0,
		.use_stc_timestamp = MMAL_PARAM_TIMESTAMP_MODE_RESET_STC
	};
	mmal_port_parameter_set(camera->control, &cam_config.hdr);
}

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
