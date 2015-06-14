#ifndef OPENIMMERSION_H
# define OPENIMMERSION_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "bcm_host.h"
#include "interface/vcos/vcos.h"

#include "interface/mmal/mmal.h"
#include "interface/mmal/mmal_pool.h"
#include "interface/mmal/util/mmal_default_components.h"
#include "interface/mmal/util/mmal_connection.h"
#include "interface/mmal/util/mmal_util.h"
#include "interface/mmal/util/mmal_util_params.h"

typedef struct sockaddr_in		t_sockaddr_in;
typedef struct sockaddr			t_sockaddr;
typedef struct addrinfo			t_addrinfo;
typedef struct in_addr			t_in_addr;

#define bool			char
#define true			1
#define false			0

#define WIDTH			640
#define HEIGHT			480
#define FPS				90

#define SERVER_NAME		"stealth-ubuntu"
#define SERVER_PORT		42000
#define SHUTTER_SPEED	(g_cam_options.shutter_speed)
#define THRESHOLD		(g_cam_options.threshold)

#define CAMERA			MMAL_COMPONENT_DEFAULT_CAMERA
#define PREVIEW			MMAL_COMPONENT_DEFAULT_VIDEO_RENDERER

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
	t_sockaddr_in			internet_socket;
	int						server;
	int						fps;
	int						start;
	int						connected;
}							t_data;

typedef struct				s_cam_options
{
	int						shutter_speed;
	int						threshold;
}							t_cam_options;

t_cam_options	g_cam_options;
t_data			g_data;
int				g_status;

/* commit_ports.c */
void	set_preview_port(MMAL_PORT_T * camera_preview_port);
void	set_video_port(MMAL_PORT_T * camera_video_port);
void	set_preview_input_port(void);

/* client.c */
void	wait_for_server(int fd, t_sockaddr * s_addr);
void	init_client(char * name, int port);

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
void	fetch_ports(MMAL_COMPONENT_T * camera);
void	flush_buffers(void);

/* init.c */

void	init(void);
void	init_camera(void);
void	init_preview(void);

/* debug.c */
void	sig_handler(int sig);
void	check(int status, const char * func, int line, char * msg);
void	dump(uint8_t * data, uint32_t length);
void	update_fps(void);

/* callback.c */
void	video_buffer_callback(MMAL_PORT_T * port, MMAL_BUFFER_HEADER_T * buffer);
void	preview_buffer_callback(MMAL_PORT_T * port, MMAL_BUFFER_HEADER_T * buffer);

#endif
