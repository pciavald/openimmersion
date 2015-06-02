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
	commit_port(g_data.camera_preview_port, MMAL_ENCODING_I420, 2, "preview");
	commit_port(g_data.camera_video_port, MMAL_ENCODING_OPAQUE, 2, "video");
	create_pool_on_port(
			&(g_data.camera_video_port_pool),
			g_data.camera_video_port,
			video_buffer_callback,
			"video");
	enable_component(g_data.camera, "camera");
}

int		main(int argc, char ** argv)
{
	MMAL_ES_FORMAT_T *		format;

	bcm_host_init();
	bzero((void *)&g_data, sizeof (g_data));

	init_camera();

	create_component(PREVIEW, &(g_data.preview), "preview");

	configure_preview_input();

	commit_port(
			g_data.preview_input_port,
			MMAL_ENCODING_I420,
			4,
			"preview_input");

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

