#include "openimmersion.h"

void	init(void)
{
	signal(SIGINT, sig_handler);
	signal(SIGPIPE, sig_handler);
	bcm_host_init();
	bzero((void *)&g_data, sizeof (g_data));
	SHUTTER_SPEED = 3000;
	THRESHOLD = 750;
	fprintf(stderr, "starting program with %ix%i %ifps shutter:%i thrs:%i\n",
			WIDTH, HEIGHT, FPS, SHUTTER_SPEED, THRESHOLD);
}

void	init_camera(void)
{
	fprintf(stderr, "initializing camera\n");
	create_component(CAMERA, &(g_data.camera), "camera");
	fetch_ports(g_data.camera);
	configure_camera(g_data.camera);
	set_preview_port(g_data.camera_preview_port);
	set_video_port(g_data.camera_video_port);
	if (CALIBRATE == true)
	{
		create_pool_on_port(
			&(g_data.camera_video_port_pool),
			g_data.camera_video_port,
			calibrate,
			"video");
	}
	else {
		create_pool_on_port(
			&(g_data.camera_video_port_pool),
			g_data.camera_video_port,
			video_buffer_callback,
			"video");
	}
	enable_component(g_data.camera, "camera");
}

void	init_preview(void)
{
	fprintf(stderr, "initializing preview\n");
	create_component(PREVIEW, &(g_data.preview), "preview");
	configure_preview_input();
	set_preview_input_port();
	create_pool_on_port(
			&(g_data.preview_input_port_pool),
			g_data.preview_input_port,
			preview_buffer_callback,
			"preview");
	flush_buffers();
}
