#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <bcm_host.h>

#include <interface/vcos/vcos_semaphore.h>
#include <interface/vmcs_host/vchost.h>

#include <IL/OMX_Core.h>
#include <IL/OMX_Component.h>
#include <IL/OMX_Video.h>
#include <IL/OMX_Broadcom.h>

// Hard coded parameters
#define VIDEO_WIDTH                     1920 / 4
#define VIDEO_HEIGHT                    1080 / 4
#define VIDEO_FRAMERATE                 50
#define CAM_DEVICE_NUMBER               0
#define CAM_SHARPNESS                   0                       // -100 .. 100
#define CAM_CONTRAST                    0                       // -100 .. 100
#define CAM_BRIGHTNESS                  50                      // 0 .. 100
#define CAM_SATURATION                  0                       // -100 .. 100
#define CAM_EXPOSURE_VALUE_COMPENSTAION 0
#define CAM_EXPOSURE_ISO_SENSITIVITY    100
#define CAM_EXPOSURE_AUTO_SENSITIVITY   OMX_FALSE
#define CAM_FRAME_STABILISATION         OMX_TRUE
#define CAM_WHITE_BALANCE_CONTROL       OMX_WhiteBalControlAuto // OMX_WHITEBALCONTROLTYPE
#define CAM_IMAGE_FILTER                OMX_ImageFilterNoise    // OMX_IMAGEFILTERTYPE
#define CAM_FLIP_HORIZONTAL             OMX_FALSE
#define CAM_FLIP_VERTICAL               OMX_TRUE

// Dunno where this is originally stolen from...
#define OMX_INIT_STRUCTURE(a) \
	memset(&(a), 0, sizeof(a)); \
(a).nSize = sizeof(a); \
(a).nVersion.nVersion = OMX_VERSION; \
(a).nVersion.s.nVersionMajor = OMX_VERSION_MAJOR; \
(a).nVersion.s.nVersionMinor = OMX_VERSION_MINOR; \
(a).nVersion.s.nRevision = OMX_VERSION_REVISION; \
(a).nVersion.s.nStep = OMX_VERSION_STEP

#include <sys/time.h>
#include <math.h>

struct timeval tvBegin, tvEnd, tvDiff;

int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1)
{
	long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
	result->tv_sec = diff / 1000000;
	result->tv_usec = diff % 1000000;

	return (diff < 0);
}

// buffer use
int used = 0;
static void use_buffer(char* buf)
{
	gettimeofday(&tvEnd, NULL);
	timeval_subtract(&tvDiff, &tvEnd, &tvBegin);
	fprintf(stderr, "%ld.%06ld\n", tvDiff.tv_sec, tvDiff.tv_usec);
	gettimeofday(&tvBegin, NULL);
	if (used++ == 10)
		fprintf(stdout, buf);
}

typedef struct
{
	OMX_HANDLETYPE			camera;
	OMX_BUFFERHEADERTYPE	*camera_ppBuffer_in;
	OMX_BUFFERHEADERTYPE	*camera_ppBuffer_out;
	int						camera_ready;
	int						camera_output_buffer_available;
	OMX_HANDLETYPE			null_sink;
	int						flushed;
	FILE					*fd_out;
	VCOS_SEMAPHORE_T		handler_lock;
} appctx;

typedef struct
{
	int		width;
	int		height;
	size_t	size;
	int		buf_stride;
	int 	buf_slice_height;
	int 	buf_extra_padding;
	int 	p_offset[3];
	int 	p_stride[3];
} i420_frame_info;

#define ROUND_UP_2(num) (((num)+1)&~1)
#define ROUND_UP_4(num) (((num)+3)&~3)
static void get_i420_frame_info(
		int width,
		int height,
		int buf_stride,
		int buf_slice_height,
		i420_frame_info *info)
{
	info->p_stride[0] = ROUND_UP_4(width);
	info->p_stride[1] = ROUND_UP_4(ROUND_UP_2(width) / 2);
	info->p_stride[2] = info->p_stride[1];
	info->p_offset[0] = 0;
	info->p_offset[1] = info->p_stride[0] * ROUND_UP_2(height);
	info->p_offset[2] = info->p_offset[1] + info->p_stride[1] * (ROUND_UP_2(height) / 2);
	info->size = info->p_offset[2] + info->p_stride[2] * (ROUND_UP_2(height) / 2);
	info->width = width;
	info->height = height;
	info->buf_stride = buf_stride;
	info->buf_slice_height = buf_slice_height;
	info->buf_extra_padding =
		buf_slice_height >= 0
		? ((buf_slice_height && (height % buf_slice_height))
				? (buf_slice_height - (height % buf_slice_height))
				: 0)
		: -1;
}

static void block_until_state_changed(
		OMX_HANDLETYPE hComponent,
		OMX_STATETYPE wanted_eState)
{
	OMX_STATETYPE eState;
	int i = 0;
	while(i++ == 0 || eState != wanted_eState)
	{
		OMX_GetState(hComponent, &eState);
		if(eState != wanted_eState)
			usleep(10000);
	}
}

static void block_until_port_changed(
		OMX_HANDLETYPE hComponent,
		OMX_U32 nPortIndex,
		OMX_BOOL bEnabled)
{
	OMX_ERRORTYPE r;
	OMX_PARAM_PORTDEFINITIONTYPE portdef;
	OMX_INIT_STRUCTURE(portdef);
	portdef.nPortIndex = nPortIndex;
	OMX_U32 i = 0;
	while(i++ == 0 || portdef.bEnabled != bEnabled) {
		r = OMX_GetParameter(hComponent, OMX_IndexParamPortDefinition, &portdef);
		if(portdef.bEnabled != bEnabled) {
			usleep(10000);
		}
	}
}

static void block_until_flushed(appctx *ctx)
{
	int quit;
	while(!quit)
	{
		vcos_semaphore_wait(&ctx->handler_lock);
		if(ctx->flushed)
		{
			ctx->flushed = 0;
			quit = 1;
		}
		vcos_semaphore_post(&ctx->handler_lock);
		if(!quit)
			usleep(10000);
	}
}

static void init_component_handle(
		const char *name,
		OMX_HANDLETYPE* hComponent,
		OMX_PTR pAppData,
		OMX_CALLBACKTYPE* callbacks)
{
	OMX_ERRORTYPE r;
	char fullname[32];

	// Get handle
	memset(fullname, 0, sizeof(fullname));
	strcat(fullname, "OMX.broadcom.");
	strncat(fullname, name, strlen(fullname) - 1);
	r = OMX_GetHandle(hComponent, fullname, pAppData, callbacks);

	// Disable ports
	OMX_INDEXTYPE types[] = {
		OMX_IndexParamAudioInit,
		OMX_IndexParamVideoInit,
		OMX_IndexParamImageInit,
		OMX_IndexParamOtherInit
	};
	OMX_PORT_PARAM_TYPE ports;
	OMX_INIT_STRUCTURE(ports);
	OMX_GetParameter(*hComponent, OMX_IndexParamVideoInit, &ports);

	int i;
	for(i = 0; i < 4; i++) {
		if(OMX_GetParameter(*hComponent, types[i], &ports) == OMX_ErrorNone)
		{
			OMX_U32 nPortIndex;
			for(nPortIndex = ports.nStartPortNumber;
					nPortIndex < ports.nStartPortNumber + ports.nPorts;
					nPortIndex++)
			{
				r = OMX_SendCommand(*hComponent, OMX_CommandPortDisable, nPortIndex, NULL);
				block_until_port_changed(*hComponent, nPortIndex, OMX_FALSE);
			}
		}
	}
}

static int want_quit = 0;
static void signal_handler(int signal) {
	want_quit = 1;
}

static OMX_ERRORTYPE event_handler(
		OMX_HANDLETYPE hComponent,
		OMX_PTR pAppData,
		OMX_EVENTTYPE eEvent,
		OMX_U32 nData1,
		OMX_U32 nData2,
		OMX_PTR pEventData)
{
	appctx *ctx = (appctx *)pAppData;

	switch(eEvent) {
		case OMX_EventCmdComplete:
			vcos_semaphore_wait(&ctx->handler_lock);
			if(nData1 == OMX_CommandFlush) {
				ctx->flushed = 1;
			}
			vcos_semaphore_post(&ctx->handler_lock);
			break;
		case OMX_EventParamOrConfigChanged:
			vcos_semaphore_wait(&ctx->handler_lock);
			if(nData2 == OMX_IndexParamCameraDeviceNumber) {
				ctx->camera_ready = 1;
			}
			vcos_semaphore_post(&ctx->handler_lock);
			break;
		case OMX_EventError:
			break;
		default:
			break;
	}

	return OMX_ErrorNone;
}

static OMX_ERRORTYPE fill_output_buffer_done_handler(
		OMX_HANDLETYPE hComponent,
		OMX_PTR pAppData,
		OMX_BUFFERHEADERTYPE* pBuffer)
{
	appctx *ctx = ((appctx*)pAppData);
	vcos_semaphore_wait(&ctx->handler_lock);
	ctx->camera_output_buffer_available = 1;
	vcos_semaphore_post(&ctx->handler_lock);
	return OMX_ErrorNone;
}

int main(int argc, char **argv)
{
	gettimeofday(&tvBegin, NULL);
	bcm_host_init();

	OMX_ERRORTYPE r;

	r = OMX_Init();

	// Init context
	appctx ctx;
	memset(&ctx, 0, sizeof(ctx));
	vcos_semaphore_create(&ctx.handler_lock, "handler_lock", 1);

	// Init component handles
	OMX_CALLBACKTYPE callbacks;
	memset(&ctx, 0, sizeof(callbacks));
	callbacks.EventHandler   = event_handler;
	callbacks.FillBufferDone = fill_output_buffer_done_handler;

	init_component_handle("camera", &ctx.camera , &ctx, &callbacks);
	init_component_handle("null_sink", &ctx.null_sink, &ctx, &callbacks);

	// Request a callback to be made when OMX_IndexParamCameraDeviceNumber is
	// changed signaling that the camera device is ready for use.
	OMX_CONFIG_REQUESTCALLBACKTYPE cbtype;
	OMX_INIT_STRUCTURE(cbtype);
	cbtype.nPortIndex = OMX_ALL;
	cbtype.nIndex     = OMX_IndexParamCameraDeviceNumber;
	cbtype.bEnable    = OMX_TRUE;
	r = OMX_SetConfig(ctx.camera, OMX_IndexConfigRequestCallback, &cbtype);

	// Set device number, this triggers the callback configured just above
	OMX_PARAM_U32TYPE device;
	OMX_INIT_STRUCTURE(device);
	device.nPortIndex = OMX_ALL;
	device.nU32 = CAM_DEVICE_NUMBER;
	r = OMX_SetParameter(ctx.camera, OMX_IndexParamCameraDeviceNumber, &device);

	// Configure video format emitted by camera preview output port
	OMX_PARAM_PORTDEFINITIONTYPE camera_portdef;
	OMX_INIT_STRUCTURE(camera_portdef);
	camera_portdef.nPortIndex = 70;
	r = OMX_GetParameter(ctx.camera, OMX_IndexParamPortDefinition, &camera_portdef);
	camera_portdef.format.video.nFrameWidth  = VIDEO_WIDTH;
	camera_portdef.format.video.nFrameHeight = VIDEO_HEIGHT;
	camera_portdef.format.video.xFramerate   = VIDEO_FRAMERATE << 16;
	camera_portdef.format.video.nStride      =
		(camera_portdef.format.video.nFrameWidth
		 + camera_portdef.nBufferAlignment - 1)
		& (~(camera_portdef.nBufferAlignment - 1));
	camera_portdef.format.video.eColorFormat = OMX_COLOR_FormatYUV420PackedPlanar;
	r = OMX_SetParameter(ctx.camera, OMX_IndexParamPortDefinition, &camera_portdef);

	// Configure video format emitted by camera video output port
	// Use configuration from camera preview output as basis for
	// camera video output configuration
	OMX_INIT_STRUCTURE(camera_portdef);
	camera_portdef.nPortIndex = 70;
	r = OMX_GetParameter(ctx.camera, OMX_IndexParamPortDefinition, &camera_portdef);
	camera_portdef.nPortIndex = 71;
	r = OMX_SetParameter(ctx.camera, OMX_IndexParamPortDefinition, &camera_portdef);

	// Configure frame rate
	OMX_CONFIG_FRAMERATETYPE framerate;
	OMX_INIT_STRUCTURE(framerate);
	framerate.nPortIndex = 70;
	framerate.xEncodeFramerate = camera_portdef.format.video.xFramerate;
	r = OMX_SetConfig(ctx.camera, OMX_IndexConfigVideoFramerate, &framerate);
	framerate.nPortIndex = 71;
	r = OMX_SetConfig(ctx.camera, OMX_IndexConfigVideoFramerate, &framerate);

	// Configure sharpness
	OMX_CONFIG_SHARPNESSTYPE sharpness;
	OMX_INIT_STRUCTURE(sharpness);
	sharpness.nPortIndex = OMX_ALL;
	sharpness.nSharpness = CAM_SHARPNESS;
	r = OMX_SetConfig(ctx.camera, OMX_IndexConfigCommonSharpness, &sharpness);

	// Configure contrast
	OMX_CONFIG_CONTRASTTYPE contrast;
	OMX_INIT_STRUCTURE(contrast);
	contrast.nPortIndex = OMX_ALL;
	contrast.nContrast = CAM_CONTRAST;
	r = OMX_SetConfig(ctx.camera, OMX_IndexConfigCommonContrast, &contrast);

	// Configure saturation
	OMX_CONFIG_SATURATIONTYPE saturation;
	OMX_INIT_STRUCTURE(saturation);
	saturation.nPortIndex = OMX_ALL;
	saturation.nSaturation = CAM_SATURATION;
	r = OMX_SetConfig(ctx.camera, OMX_IndexConfigCommonSaturation, &saturation);

	// Configure brightness
	OMX_CONFIG_BRIGHTNESSTYPE brightness;
	OMX_INIT_STRUCTURE(brightness);
	brightness.nPortIndex = OMX_ALL;
	brightness.nBrightness = CAM_BRIGHTNESS;
	r = OMX_SetConfig(ctx.camera, OMX_IndexConfigCommonBrightness, &brightness);

	// Configure exposure value
	OMX_CONFIG_EXPOSUREVALUETYPE exposure_value;
	OMX_INIT_STRUCTURE(exposure_value);
	exposure_value.nPortIndex = OMX_ALL;
	exposure_value.xEVCompensation = CAM_EXPOSURE_VALUE_COMPENSTAION;
	exposure_value.bAutoSensitivity = CAM_EXPOSURE_AUTO_SENSITIVITY;
	exposure_value.nSensitivity = CAM_EXPOSURE_ISO_SENSITIVITY;
	r = OMX_SetConfig(ctx.camera, OMX_IndexConfigCommonExposureValue, &exposure_value);

	// Configure frame frame stabilisation
	OMX_CONFIG_FRAMESTABTYPE frame_stabilisation_control;
	OMX_INIT_STRUCTURE(frame_stabilisation_control);
	frame_stabilisation_control.nPortIndex = OMX_ALL;
	frame_stabilisation_control.bStab = CAM_FRAME_STABILISATION;
	r = OMX_SetConfig(ctx.camera, OMX_IndexConfigCommonFrameStabilisation, &frame_stabilisation_control);

	// Configure frame white balance control
	OMX_CONFIG_WHITEBALCONTROLTYPE white_balance_control;
	OMX_INIT_STRUCTURE(white_balance_control);
	white_balance_control.nPortIndex = OMX_ALL;
	white_balance_control.eWhiteBalControl = CAM_WHITE_BALANCE_CONTROL;
	r = OMX_SetConfig(ctx.camera, OMX_IndexConfigCommonWhiteBalance, &white_balance_control);

	// Configure image filter
	OMX_CONFIG_IMAGEFILTERTYPE image_filter;
	OMX_INIT_STRUCTURE(image_filter);
	image_filter.nPortIndex = OMX_ALL;
	image_filter.eImageFilter = CAM_IMAGE_FILTER;
	r = OMX_SetConfig(ctx.camera, OMX_IndexConfigCommonImageFilter, &image_filter);

	// Configure mirror
	OMX_MIRRORTYPE eMirror = OMX_MirrorNone;
	if(CAM_FLIP_HORIZONTAL && !CAM_FLIP_VERTICAL)
		eMirror = OMX_MirrorHorizontal;
	else if(!CAM_FLIP_HORIZONTAL && CAM_FLIP_VERTICAL)
		eMirror = OMX_MirrorVertical;
	else if(CAM_FLIP_HORIZONTAL && CAM_FLIP_VERTICAL)
		eMirror = OMX_MirrorBoth;
	OMX_CONFIG_MIRRORTYPE mirror;
	OMX_INIT_STRUCTURE(mirror);
	mirror.nPortIndex = 71;
	mirror.eMirror = eMirror;
	r = OMX_SetConfig(ctx.camera, OMX_IndexConfigCommonMirror, &mirror);

	// Ensure camera is ready
	while(!ctx.camera_ready) {
		usleep(10000);
	}

	// Tunnel camera preview output port and null sink input port
	r = OMX_SetupTunnel(ctx.camera, 70, ctx.null_sink, 240);

	// Switch components to idle state
	r = OMX_SendCommand(ctx.camera, OMX_CommandStateSet, OMX_StateIdle, NULL);
	block_until_state_changed(ctx.camera, OMX_StateIdle);
	r = OMX_SendCommand(ctx.null_sink, OMX_CommandStateSet, OMX_StateIdle, NULL);
	block_until_state_changed(ctx.null_sink, OMX_StateIdle);

	// Enable ports
	r = OMX_SendCommand(ctx.camera, OMX_CommandPortEnable, 73, NULL);
	block_until_port_changed(ctx.camera, 73, OMX_TRUE);
	r = OMX_SendCommand(ctx.camera, OMX_CommandPortEnable, 70, NULL);
	block_until_port_changed(ctx.camera, 70, OMX_TRUE);
	r = OMX_SendCommand(ctx.camera, OMX_CommandPortEnable, 71, NULL);
	block_until_port_changed(ctx.camera, 71, OMX_TRUE);
	r = OMX_SendCommand(ctx.null_sink, OMX_CommandPortEnable, 240, NULL);
	block_until_port_changed(ctx.null_sink, 240, OMX_TRUE);

	// Allocate camera input and video output buffers,
	// buffers for tunneled ports are allocated internally by OMX
	OMX_INIT_STRUCTURE(camera_portdef);
	camera_portdef.nPortIndex = 73;
	r = OMX_GetParameter(ctx.camera, OMX_IndexParamPortDefinition, &camera_portdef);
	r = OMX_AllocateBuffer(ctx.camera, &ctx.camera_ppBuffer_in, 73, NULL, camera_portdef.nBufferSize);
	camera_portdef.nPortIndex = 71;
	r = OMX_GetParameter(ctx.camera, OMX_IndexParamPortDefinition, &camera_portdef);
	r = OMX_AllocateBuffer(ctx.camera, &ctx.camera_ppBuffer_out, 71, NULL, camera_portdef.nBufferSize);

	// Just use stdout for output
	ctx.fd_out = stdout;

	// Switch state of the components prior to starting
	// the video capture loop
	r = OMX_SendCommand(ctx.camera, OMX_CommandStateSet, OMX_StateExecuting, NULL);
	block_until_state_changed(ctx.camera, OMX_StateExecuting);
	r = OMX_SendCommand(ctx.null_sink, OMX_CommandStateSet, OMX_StateExecuting, NULL);
	block_until_state_changed(ctx.null_sink, OMX_StateExecuting);

	// Start capturing video with the camera
	OMX_CONFIG_PORTBOOLEANTYPE capture;
	OMX_INIT_STRUCTURE(capture);
	capture.nPortIndex = 71;
	capture.bEnabled = OMX_TRUE;
	r = OMX_SetParameter(ctx.camera, OMX_IndexConfigPortCapturing, &capture);

	// Buffer representing an I420 frame where to unpack
	// the fragmented Y, U, and V plane spans from the OMX buffers
	i420_frame_info frame_info, buf_info;
	get_i420_frame_info(camera_portdef.format.image.nFrameWidth, camera_portdef.format.image.nFrameHeight, camera_portdef.format.image.nStride, camera_portdef.format.video.nSliceHeight, &frame_info);
	get_i420_frame_info(frame_info.buf_stride, frame_info.buf_slice_height, -1, -1, &buf_info);
	char *frame = calloc(1, frame_info.size);

	// Some counters
	int frame_num = 1, buf_num = 0;
	size_t output_written, frame_bytes = 0, buf_size, buf_bytes_read = 0, buf_bytes_copied;
	int i;
	// I420 spec: U and V plane span size half of the size of the Y plane span size
	int max_spans_y = buf_info.height, max_spans_uv = max_spans_y / 2;
	int valid_spans_y, valid_spans_uv;
	// For unpack memory copy operation
	unsigned char *buf_start;
	int max_spans, valid_spans;
	int dst_offset, src_offset, span_size;
	// For controlling the loop
	int quit_detected = 0, quit_in_frame_boundry = 0, need_next_buffer_to_be_filled = 1;

	signal(SIGINT,  signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGQUIT, signal_handler);

	while(42)
	{
		if(ctx.camera_output_buffer_available)
		{
			if(want_quit && !quit_detected)
			{
				quit_detected = 1;
				quit_in_frame_boundry = ctx.camera_ppBuffer_out->nFlags & OMX_BUFFERFLAG_ENDOFFRAME;
			}
			if(quit_detected &&
					(quit_in_frame_boundry ^
					 (ctx.camera_ppBuffer_out->nFlags & OMX_BUFFERFLAG_ENDOFFRAME)))
				break;
			buf_start = ctx.camera_ppBuffer_out->pBuffer + ctx.camera_ppBuffer_out->nOffset;
			buf_size = ctx.camera_ppBuffer_out->nFilledLen;
			buf_bytes_read += buf_size;
			buf_bytes_copied = 0;
			valid_spans_y = max_spans_y
				- ((ctx.camera_ppBuffer_out->nFlags & OMX_BUFFERFLAG_ENDOFFRAME)
						? frame_info.buf_extra_padding
						: 0);
			valid_spans_uv = valid_spans_y / 2;
			for(i = 0; i < 3; i++)
			{
				max_spans   = (i == 0 ? max_spans_y   : max_spans_uv);
				valid_spans = (i == 0 ? valid_spans_y : valid_spans_uv);
				dst_offset =
					frame_info.p_offset[i] +
					(buf_num * frame_info.p_stride[i] * max_spans);
				src_offset =
					buf_info.p_offset[i];
				span_size =
					frame_info.p_stride[i] * valid_spans;
				memcpy(
						frame + dst_offset,
						buf_start + src_offset,
						span_size);
				buf_bytes_copied += span_size;
			}
			frame_bytes += buf_bytes_copied;
			buf_num++;
			if(ctx.camera_ppBuffer_out->nFlags & OMX_BUFFERFLAG_ENDOFFRAME)
			{
				use_buffer(frame);
				frame_num++;
				buf_num = 0;
				buf_bytes_read = 0;
				frame_bytes = 0;
				memset(frame, 0, frame_info.size);
			}
			need_next_buffer_to_be_filled = 1;
		}
		if(need_next_buffer_to_be_filled)
		{
			need_next_buffer_to_be_filled = 0;
			ctx.camera_output_buffer_available = 0;
			r = OMX_FillThisBuffer(ctx.camera, ctx.camera_ppBuffer_out);
		}
		usleep(10);
	}

	// Restore signal handlers
	signal(SIGINT,  SIG_DFL);
	signal(SIGTERM, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);

	// Stop capturing video with the camera
	OMX_INIT_STRUCTURE(capture);
	capture.nPortIndex = 71;
	capture.bEnabled = OMX_FALSE;
	r = OMX_SetParameter(ctx.camera, OMX_IndexConfigPortCapturing, &capture);

	// Return the last full buffer back to the camera component
	r = OMX_FillThisBuffer(ctx.camera, ctx.camera_ppBuffer_out);

	// Flush the buffers on each component
	r = OMX_SendCommand(ctx.camera, OMX_CommandFlush, 73, NULL);
	block_until_flushed(&ctx);
	r = OMX_SendCommand(ctx.camera, OMX_CommandFlush, 70, NULL);
	block_until_flushed(&ctx);
	r = OMX_SendCommand(ctx.camera, OMX_CommandFlush, 71, NULL);
	block_until_flushed(&ctx);
	r = OMX_SendCommand(ctx.null_sink, OMX_CommandFlush, 240, NULL);
	block_until_flushed(&ctx);

	// Disable all the ports
	r = OMX_SendCommand(ctx.camera, OMX_CommandPortDisable, 73, NULL);
	block_until_port_changed(ctx.camera, 73, OMX_FALSE);
	r = OMX_SendCommand(ctx.camera, OMX_CommandPortDisable, 70, NULL);
	block_until_port_changed(ctx.camera, 70, OMX_FALSE);
	r = OMX_SendCommand(ctx.camera, OMX_CommandPortDisable, 71, NULL);
	block_until_port_changed(ctx.camera, 71, OMX_FALSE);
	r = OMX_SendCommand(ctx.null_sink, OMX_CommandPortDisable, 240, NULL);
	block_until_port_changed(ctx.null_sink, 240, OMX_FALSE);

	// Free all the buffers
	r = OMX_FreeBuffer(ctx.camera, 73, ctx.camera_ppBuffer_in);
	r = OMX_FreeBuffer(ctx.camera, 71, ctx.camera_ppBuffer_out);

	// Transition all the components to idle and then to loaded states
	r = OMX_SendCommand(ctx.camera, OMX_CommandStateSet, OMX_StateIdle, NULL);
	block_until_state_changed(ctx.camera, OMX_StateIdle);
	r = OMX_SendCommand(ctx.null_sink, OMX_CommandStateSet, OMX_StateIdle, NULL);
	block_until_state_changed(ctx.null_sink, OMX_StateIdle);
	r = OMX_SendCommand(ctx.camera, OMX_CommandStateSet, OMX_StateLoaded, NULL);
	block_until_state_changed(ctx.camera, OMX_StateLoaded);
	r = OMX_SendCommand(ctx.null_sink, OMX_CommandStateSet, OMX_StateLoaded, NULL);
	block_until_state_changed(ctx.null_sink, OMX_StateLoaded);

	// Free the component handles
	r = OMX_FreeHandle(ctx.camera);
	r = OMX_FreeHandle(ctx.null_sink);

	// Exit
	fclose(ctx.fd_out);
	free(frame);

	vcos_semaphore_delete(&ctx.handler_lock);
	r = OMX_Deinit();

	return 0;
}
