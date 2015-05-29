/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pciavald <pciavald@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/05/28 12:44:31 by pciavald          #+#    #+#             */
/*   Updated: 2015/05/28 12:45:12 by pciavald         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include "/opt/vc/include/interface/mmal/mmal.h"

static void got_buffer(MMAL_BUFFER_HEADER_T *buffer)
{
	(void)buffer;
}

static void check(MMAL_STATUS_T status)
{
	if (status == MMAL_SUCCESS)
		return ;
	fprintf(stderr, "error status %i\n", status);
	exit(status);
}

// The decoder is done with the data, just recycle the buffer header into its pool
static void input_callback(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer)
{
	mmal_buffer_header_release(buffer);
}

static void output_callback(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer)
{
	MMAL_QUEUE_T *queue = (MMAL_QUEUE_T *)port->userdata;
	mmal_queue_put(queue, buffer); // Queue the decoded video frame
}


int		main(int argc, char** argv)
{
	MMAL_COMPONENT_T	*decoder = 0;
	MMAL_STATUS_T		status;

	// Create the video decoder component on VideoCore
	status = mmal_component_create("vc.ril.video_decoder", &decoder);
	check(status);

	// Set format of video decoder input port
	MMAL_ES_FORMAT_T *format_in = decoder->input[0]->format;
	format_in->type						= MMAL_ES_TYPE_VIDEO;
	format_in->encoding					= MMAL_ENCODING_H264;
	format_in->es->video.width			= 1280;
	format_in->es->video.height			= 720;
	format_in->es->video.frame_rate.num	= 30;
	format_in->es->video.frame_rate.den	= 1;
	format_in->es->video.par.num		= 1;
	format_in->es->video.par.den		= 1;
	format_in->flags					= MMAL_ES_FORMAT_FLAG_FRAMED;
	//status = mmal_format_extradata_alloc(format_in, YOUR_H264_CODEC_HEADER_BYTES_SIZE);
	//check(status);
	//format_in->extradata_size = YOUR_H264_CODEC_HEADER_BYTES_SIZE;
	//memcpy(format_in->extradata, YOUR_H264_CODEC_HEADER_BYTES, format_in->extradata_size);

	status = mmal_port_format_commit(decoder->input[0]);
	check(status);

	// Once the call to mmal_port_format_commit() on the input port returns, the decoder will
	// have set the format of the output port.
	// If the decoder still doesn t have enough information to determine the format of the
	// output port, the encoding will be set to unknown. As soon as the decoder receives
	// enough stream data to determine the format of the output port it will send an event
	// to the client to signal that the format of the port has changed.
	// However, for the sake of simplicity this example assumes that the decoder was given
	// all the necessary information right at the start (i.e. video format and codec header bytes)
	MMAL_ES_FORMAT_T *format_out = decoder->output[0]->format;
	if (format_out->encoding == MMAL_ENCODING_UNKNOWN)
		check(-1);

	// Now we know the format of both ports and the requirements of the decoder, we can create
	// our buffer headers and their associated memory buffers. We use the buffer pool API for this.
	decoder->input[0]->buffer_num = decoder->input[0]->buffer_num_min;
	decoder->input[0]->buffer_size = decoder->input[0]->buffer_size_min;
	MMAL_POOL_T *pool_in = mmal_pool_create(decoder->input[0]->buffer_num,
			decoder->input[0]->buffer_size);
	decoder->output[0]->buffer_num = decoder->output[0]->buffer_num_min;
	decoder->output[0]->buffer_size = decoder->output[0]->buffer_size_min;
	MMAL_POOL_T *pool_out = mmal_pool_create(decoder->output[0]->buffer_num,
			decoder->output[0]->buffer_size);

	// Create a queue to store our decoded video frames. The callback we will get when
	// a frame has been decoded will put the frame into this queue.
	MMAL_QUEUE_T *queue_decoded_frames = mmal_queue_create();
	decoder->output[0]->userdata = (void *)queue_decoded_frames;

	// Enable all the input port and the output port.
	// The callback specified here is the function which will be called when the buffer header
	// we sent to the component has been processed.
	status = mmal_port_enable(decoder->input[0], input_callback);
	check(status);
	status = mmal_port_enable(decoder->output[0], output_callback);
	check(status);

	// Enable the component. Components will only process data when they are enabled.
	status = mmal_component_enable(decoder);
	check(status);

	// Data processing loop
	while (42)
	{
		MMAL_BUFFER_HEADER_T *buffer;

		// The client needs to implement its own blocking code.
		// (e.g. a semaphore which is posted when a buffer header is put in one of the queues)
		WAIT_FOR_QUEUES_TO_HAVE_BUFFERS();

		// Send empty buffers to the output port of the decoder to allow the decoder to start
		// producing frames as soon as it gets input data
		while ((buffer = mmal_queue_get(pool_out->queue)) != NULL)
		{
			status = mmal_port_send_buffer(decoder->output[0], buffer);
			check(status);
		}

		// Send data to decode to the input port of the video decoder
		if ((buffer = mmal_queue_get(pool_in->queue)) != NULL)
		{
			READ_DATA_INTO_BUFFER(buffer);

			status = mmal_port_send_buffer(decoder->input[0], buffer);
			check(status);
		}

		// Get our decoded frames. We also need to cope with events
		// generated from the component here.
		while ((buffer = mmal_queue_get(queue_decoded_frames)) != NULL)
		{
			if (buffer->cmd)
			{
				// This is an event. Do something with it and release the buffer.
				mmal_buffer_header_release(buffer);
				continue;
			}

			// We have a frame, do something with it (why not display it for instance?).
			// Once we're done with it, we release it. It will magically go back
			// to its original pool so it can be reused for a new video frame.
			got_buffer(buffer);
			mmal_buffer_header_release(buffer);
		}
	}
	// Cleanup everything
	mmal_component_destroy(decoder);
	mmal_pool_destroy(pool_in);
	mmal_pool_destroy(pool_out);
	mmal_queue_destroy(queue_decoded_frames);
	return (0);
}
