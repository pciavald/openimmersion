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
	mmal_port_parameter_set_uint32(camera->control,
			MMAL_PARAMETER_SHUTTER_SPEED, 2500);
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

void	fetch_ports(MMAL_COMPONENT_T * camera)
{
	if (g_data.camera->port_num)
	{
		g_data.camera_preview_port	= camera->output[MMAL_CAMERA_PREVIEW_PORT];
		g_data.camera_video_port	= camera->output[MMAL_CAMERA_VIDEO_PORT];
	}
	else
		check(-1, __func__, __LINE__, "camera has no ports");
}

void	flush_buffers(void)
{
	MMAL_BUFFER_HEADER_T *	buffer;
	int						size;
	int						i;

	size = mmal_queue_length(g_data.camera_video_port_pool->queue);
	for (i = 0; i < size; i++)
	{
		buffer = mmal_queue_get(g_data.camera_video_port_pool->queue);
		if (buffer == NULL)
			check(-1, __func__, __LINE__, "flushing buffers");
		g_status = mmal_port_send_buffer(g_data.camera_video_port, buffer);
		check(g_status, __func__, __LINE__, "sending flushed buffers");
	}
}
