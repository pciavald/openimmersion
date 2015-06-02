#include "openimmersion.h"

void	init_camera(void)
{
	create_component(CAMERA, &(g_data.camera), "camera");
	fetch_ports(g_data.camera);
	configure_camera(g_data.camera);
	commit_port(g_data.camera_preview_port, I420, 2, "preview");
	commit_port(g_data.camera_video_port, OPAQUE, 2, "video");
	create_pool_on_port(
			&(g_data.camera_video_port_pool),
			g_data.camera_video_port,
			video_buffer_callback,
			"video");
	enable_component(g_data.camera, "camera");
}

void	init_preview(void)
{
	create_component(PREVIEW, &(g_data.preview), "preview");
	configure_preview_input();
	commit_port(g_data.preview_input_port, I420, 4, "preview_input");
	create_pool_on_port(
			&(g_data.preview_input_port_pool),
			g_data.preview_input_port,
			preview_buffer_callback,
			"preview");
	flush_buffers();
}

int		main(int argc, char ** argv)
{
	bcm_host_init();
	bzero((void *)&g_data, sizeof (g_data));
	init_camera();
	init_preview();
	start_capture();
	while (42)
		;
	return (0);
}
