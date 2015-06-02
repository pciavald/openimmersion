#include "openimmersion.h"

void	configure_camera(MMAL_COMPONENT_T * camera)
{
	MMAL_PARAMETER_CAMERA_CONFIG_T	cam_config =
	{
		{MMAL_PARAMETER_CAMERA_CONFIG, sizeof (cam_config)},
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

void	configure_preview_input(void)
{
	MMAL_DISPLAYREGION_T		param;
	MMAL_ES_FORMAT_T *			input_format;
	MMAL_ES_FORMAT_T *			video_format = g_data.camera_video_port->format;

	g_data.preview_input_port	= g_data.preview->input[0];
	param.hdr.id				= MMAL_PARAMETER_DISPLAYREGION;
	param.hdr.size				= sizeof (MMAL_DISPLAYREGION_T);
	param.set					= MMAL_DISPLAY_SET_LAYER;
	param.layer					= 0;
	param.set					|= MMAL_DISPLAY_SET_FULLSCREEN;
	param.fullscreen			= 1;
	g_status = mmal_port_parameter_set(g_data.preview_input_port, &param.hdr);
	check(g_status, __func__, __LINE__, "could not set preview parameters");
	input_format = g_data.preview_input_port->format;
	mmal_format_copy(input_format, video_format);
}

void	commit_port(
		MMAL_PORT_T *	port,
		MMAL_FOURCC_T	encoding,
		unsigned int	headers,
		char *			msg)
{
	MMAL_ES_FORMAT_T *	format;

	format = port->format;
	format->encoding					= encoding;
	format->encoding_variant			= MMAL_ENCODING_I420;
	format->es->video.width				= WIDTH;
	format->es->video.height			= HEIGHT;
	format->es->video.crop.x			= 0;
	format->es->video.crop.y			= 0;
	format->es->video.crop.width		= WIDTH;
	format->es->video.crop.height		= HEIGHT;
	format->es->video.frame_rate.num	= FPS;
	format->es->video.frame_rate.den	= 1;
	port->buffer_size					= port->buffer_size_recommended;
	port->buffer_num					= headers;
	g_status = mmal_port_format_commit(port);
	check(g_status, __func__, __LINE__, msg);
}
