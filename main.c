#include "openimmersion.h"

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

void	init_camera(void)
{
	create_component(CAMERA, &(g_data.camera), "camera");
	fetch_ports(g_data.camera);
	configure_camera(g_data.camera);

	set_preview_port(g_data.camera_preview_port);
	set_video_port(g_data.camera_video_port);

	create_pool_on_port(
			&(g_data.camera_video_port_pool),
			g_data.camera_video_port,
			video_buffer_callback,
			"video");

	enable_component(g_data.camera, "camera");
}

int		main(int argc, char ** argv)
{
	printf("lol\n");
	MMAL_ES_FORMAT_T *		format;
	bcm_host_init();
	bzero((void *)&g_data, sizeof (g_data));

	init_camera();

	create_component(PREVIEW, &(g_data.preview), "preview");

	g_data.preview_input_port = g_data.preview->input[0];
	{
		MMAL_DISPLAYREGION_T	param;
		param.hdr.id		= MMAL_PARAMETER_DISPLAYREGION;
		param.hdr.size		= sizeof (MMAL_DISPLAYREGION_T);
		param.set			= MMAL_DISPLAY_SET_LAYER;
		param.layer			= 0;
		param.set			|= MMAL_DISPLAY_SET_FULLSCREEN;
		param.fullscreen	= 1;
		g_status = mmal_port_parameter_set(g_data.preview_input_port, &param.hdr);
		check(g_status, __func__, __LINE__, "could not set preview parameters");
	}
	mmal_format_copy(g_data.preview_input_port->format, g_data.camera_video_port->format);

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

	create_pool_on_port(
			&(g_data.preview_input_port_pool),
			g_data.preview_input_port,
			preview_buffer_callback,
			"preview");

	{
		// Send all the buffers to the encoder output port
		int num = mmal_queue_length(g_data.camera_video_port_pool->queue);
		int q;

		for (q = 0; q < num; q++) {
			MMAL_BUFFER_HEADER_T *buffer = mmal_queue_get(g_data.camera_video_port_pool->queue);

			if (!buffer)
				printf("Unable to get a required buffer %d from pool queue\n", q);

			if (mmal_port_send_buffer(g_data.camera_video_port, buffer) != MMAL_SUCCESS)
				printf("Unable to send a buffer to encoder output port (%d)\n", q);
		}
	}

	if (mmal_port_parameter_set_boolean(g_data.camera_video_port, MMAL_PARAMETER_CAPTURE, 1) != MMAL_SUCCESS) {
		printf("%s: Failed to start capture\n", __func__);
	}

	while (1);

	return 0;
}

