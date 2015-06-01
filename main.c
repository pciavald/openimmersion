#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "bcm_host.h"
#include "interface/vcos/vcos.h"

#include "interface/mmal/mmal.h"
#include "interface/mmal/util/mmal_default_components.h"
#include "interface/mmal/util/mmal_connection.h"

#define WIDTH	640
#define HEIGHT	480
#define FPS		90

#define MMAL_CAMERA_PREVIEW_PORT	0
#define MMAL_CAMERA_VIDEO_PORT		1
#define MMAL_CAMERA_CAPTURE_PORT	2

MMAL_POOL_T *	camera_video_port_pool;
MMAL_POOL_T *	preview_input_port_pool;
MMAL_PORT_T *	preview_input_port = NULL;

void	preview_buffer_callback(
		MMAL_PORT_T * port,
		MMAL_BUFFER_HEADER_T * buffer)
{
	mmal_buffer_header_release(buffer);
}

void	video_buffer_callback(
		MMAL_PORT_T * port,
		MMAL_BUFFER_HEADER_T * buffer)
{
	static int				loop = 0;
	static struct timespec	t1;
	struct timespec 		t2;
	int						d;

	if (loop == 0)
		clock_gettime(CLOCK_MONOTONIC, &t1);
	clock_gettime(CLOCK_MONOTONIC, &t2);

	d = t2.tv_sec - t1.tv_sec;

	MMAL_BUFFER_HEADER_T *	new_buffer;
	MMAL_BUFFER_HEADER_T *	preview_new_buffer;
	MMAL_POOL_T *			pool = (MMAL_POOL_T *) port->userdata;

	loop++;
	preview_new_buffer = mmal_queue_get(preview_input_port_pool->queue);

	if (preview_new_buffer)
	{
		// copy only Y
		memcpy(preview_new_buffer->data, buffer->data, WIDTH * HEIGHT);
		int i;
		int size = WIDTH * HEIGHT;
		int half = size / 2;
		int quarter = size / 4;
		for (i = size; i < size + quarter; i++)
			preview_new_buffer->data[i] = 0x00;
		for (i = size + quarter; i < size + half; i++)
			preview_new_buffer->data[i] = 0b10101010;
		preview_new_buffer->length = buffer->length;
		if (mmal_port_send_buffer(preview_input_port, preview_new_buffer) != MMAL_SUCCESS)
			printf("ERROR: Unable to send buffer \n");
	}
	else
		printf("ERROR: mmal_queue_get (%d)\n", preview_new_buffer);

	if (loop % 10 == 0)
		printf("loop = %d, Framerate = %d fps, buffer->length = %d \n", loop, loop / (d + 1), buffer->length);

	mmal_buffer_header_release(buffer);

	// and send one back to the port (if still open)
	if (port->is_enabled)
	{
		MMAL_STATUS_T status;

		new_buffer = mmal_queue_get(pool->queue);

		if (new_buffer)
			status = mmal_port_send_buffer(port, new_buffer);

		if (!new_buffer || status != MMAL_SUCCESS)
			printf("Unable to return a buffer to the video port\n");
	}
}

int		main(int argc, char ** argv)
{
	MMAL_COMPONENT_T *		camera = 0;
	MMAL_COMPONENT_T *		preview = 0;
	MMAL_ES_FORMAT_T *		format;
	MMAL_STATUS_T			status;
	MMAL_PORT_T *			camera_preview_port = NULL;
	MMAL_PORT_T *			camera_video_port = NULL;
	MMAL_PORT_T *			camera_still_port = NULL;

	MMAL_CONNECTION_T *		camera_preview_connection = 0;

	bcm_host_init();

	// creating camera component
	status = mmal_component_create(MMAL_COMPONENT_DEFAULT_CAMERA, &camera);
	if (status != MMAL_SUCCESS) {
		printf("Error: create camera %x\n", status);
		return -1;
	}

	// getting camera ports
	camera_preview_port	= camera->output[MMAL_CAMERA_PREVIEW_PORT];
	camera_video_port	= camera->output[MMAL_CAMERA_VIDEO_PORT];
	camera_still_port	= camera->output[MMAL_CAMERA_CAPTURE_PORT];

	// configuring camera
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

	// setting camera preview port
	format = camera_preview_port->format;
	format->encoding				= MMAL_ENCODING_OPAQUE;
	format->encoding_variant		= MMAL_ENCODING_I420;
	format->es->video.width			= WIDTH;
	format->es->video.height		= HEIGHT;
	format->es->video.crop.x		= 0;
	format->es->video.crop.y		= 0;
	format->es->video.crop.width	= WIDTH;
	format->es->video.crop.height	= HEIGHT;
	status = mmal_port_format_commit(camera_preview_port);
	if (status != MMAL_SUCCESS) {
		printf("Error: camera viewfinder format couldn't be set\n");
		return -1;
	}

	// setting camera video port
	//mmal_format_copy(camera_video_port->format, camera_preview_port->format);
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
	status = mmal_port_format_commit(camera_video_port);
	printf(" camera video buffer_size = %d\n", camera_video_port->buffer_size);
	printf(" camera video buffer_num = %d\n", camera_video_port->buffer_num);
	if (status != MMAL_SUCCESS) {
		printf("Error: unable to commit camera video port format (%u)\n", status);
		return -1;
	}

	// create a pool for camera video port
	camera_video_port_pool = (MMAL_POOL_T *)mmal_port_pool_create(
			camera_video_port,
			camera_video_port->buffer_num,
			camera_video_port->buffer_size);
	camera_video_port->userdata = (struct MMAL_PORT_USERDATA_T *)camera_video_port_pool;
	status = mmal_port_enable(camera_video_port, video_buffer_callback);
	if (status != MMAL_SUCCESS) {
		printf("Error: unable to enable camera video port (%u)\n", status);
		return -1;
	}

	// enable camera
	status = mmal_component_enable(camera);

	status = mmal_component_create(MMAL_COMPONENT_DEFAULT_VIDEO_RENDERER, &preview);
	if (status != MMAL_SUCCESS) {
		printf("Error: unable to create preview (%u)\n", status);
		return -1;
	}

	preview_input_port = preview->input[0];
	{
		MMAL_DISPLAYREGION_T param;
		param.hdr.id = MMAL_PARAMETER_DISPLAYREGION;
		param.hdr.size = sizeof (MMAL_DISPLAYREGION_T);
		param.set = MMAL_DISPLAY_SET_LAYER;
		param.layer = 0;
		param.set |= MMAL_DISPLAY_SET_FULLSCREEN;
		param.fullscreen = 1;
		status = mmal_port_parameter_set(preview_input_port, &param.hdr);
		if (status != MMAL_SUCCESS && status != MMAL_ENOSYS) {
			printf("Error: unable to set preview port parameters (%u)\n", status);
			return -1;
		}
	}
	mmal_format_copy(preview_input_port->format, camera_video_port->format);

	format = preview_input_port->format;

	format->encoding = MMAL_ENCODING_I420;
	format->encoding_variant = MMAL_ENCODING_I420;

	format->es->video.width = WIDTH;
	format->es->video.height = HEIGHT;
	format->es->video.crop.x = 0;
	format->es->video.crop.y = 0;
	format->es->video.crop.width = WIDTH;
	format->es->video.crop.height = HEIGHT;
	format->es->video.frame_rate.num = FPS;
	format->es->video.frame_rate.den = 1;

	preview_input_port->buffer_size = camera_video_port->buffer_size_recommended;
	preview_input_port->buffer_num = 4;

	printf(" preview buffer_size = %d\n", preview_input_port->buffer_size);
	printf(" preview buffer_num = %d\n", preview_input_port->buffer_num);

	status = mmal_port_format_commit(preview_input_port);

	preview_input_port_pool = (MMAL_POOL_T *)mmal_port_pool_create(preview_input_port, preview_input_port->buffer_num, preview_input_port->buffer_size);

	preview_input_port->userdata = (struct MMAL_PORT_USERDATA_T *) preview_input_port_pool;
	status = mmal_port_enable(preview_input_port, preview_buffer_callback);
	if (status != MMAL_SUCCESS) {
		printf("Error: unable to enable preview input port (%u)\n", status);
		return -1;
	}

	{
		// Send all the buffers to the encoder output port
		int num = mmal_queue_length(camera_video_port_pool->queue);
		int q;

		for (q = 0; q < num; q++) {
			MMAL_BUFFER_HEADER_T *buffer = mmal_queue_get(camera_video_port_pool->queue);

			if (!buffer)
				printf("Unable to get a required buffer %d from pool queue\n", q);

			if (mmal_port_send_buffer(camera_video_port, buffer) != MMAL_SUCCESS)
				printf("Unable to send a buffer to encoder output port (%d)\n", q);
		}
	}

	if (mmal_port_parameter_set_boolean(camera_video_port, MMAL_PARAMETER_CAPTURE, 1) != MMAL_SUCCESS) {
		printf("%s: Failed to start capture\n", __func__);
	}

	while (1);

	return 0;
}

