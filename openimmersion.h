#ifndef OPENIMMERSION_H
# define OPENIMMERSION_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "bcm_host.h"
#include "interface/vcos/vcos.h"

#include "interface/mmal/mmal.h"
#include "interface/mmal/util/mmal_default_components.h"
#include "interface/mmal/util/mmal_connection.h"

#define WIDTH		640
#define HEIGHT		480
#define FPS			90

#define CAMERA		MMAL_COMPONENT_DEFAULT_CAMERA
#define PREVIEW		MMAL_COMPONENT_DEFAULT_VIDEO_RENDERER
#define I420		MMAL_ENCODING_I420
#define OPAQUE		MMAL_ENCODING_OPAQUE

#define MMAL_CAMERA_PREVIEW_PORT	0
#define MMAL_CAMERA_VIDEO_PORT		1
#define MMAL_CAMERA_CAPTURE_PORT	2

typedef struct				s_data
{
	MMAL_COMPONENT_T *		camera;
	MMAL_COMPONENT_T *		preview;
	MMAL_PORT_T *			camera_video_port;
	MMAL_POOL_T *			camera_video_port_pool;
	MMAL_PORT_T *			camera_preview_port;
	MMAL_PORT_T *			preview_input_port;
	MMAL_POOL_T *			preview_input_port_pool;
	MMAL_CONNECTION_T *		camera_preview_connection;
}							t_data;

t_data			g_data;
MMAL_STATUS_T	g_status;
int				g_stop;

/* mmal.c */
void	create_component(const char * name, MMAL_COMPONENT_T ** c, char * msg);
void	enable_component(MMAL_COMPONENT_T * component, char * msg);
void	start_capture(void);
void	stop_capture(void);
void	create_pool_on_port(
		MMAL_POOL_T **		pool,
		MMAL_PORT_T *		port,
		MMAL_PORT_BH_CB_T	callback,
		char *				msg);

/* setup.c */
void	configure_camera(MMAL_COMPONENT_T * camera);
void	configure_preview_input(void);
void	commit_port(
		MMAL_PORT_T *	port,
		MMAL_FOURCC_T	encoding,
		unsigned int	headers,
		char *			msg);
void	fetch_ports(MMAL_COMPONENT_T * camera);
void	flush_buffers(void);

/* debug.c */
void	check(int status, const char * func, int line, char * msg);
void	dump(uint8_t * data, uint32_t length);

/* callback.c */
void	video_buffer_callback(
		MMAL_PORT_T *			port,
		MMAL_BUFFER_HEADER_T *	buffer);
void	preview_buffer_callback(
		MMAL_PORT_T *			port,
		MMAL_BUFFER_HEADER_T *	buffer);

#endif
