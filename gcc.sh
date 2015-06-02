#!/bin/sh
gcc \
	-I /opt/vc/userland-master/ \
	-I /opt/vc/userland-master/build/inc/interface/vcos/ \
	-I /opt/vc/userland-master/host_applications/linux/apps/raspicam/ \
	-I /opt/vc/userland-master/host_applications/linux/libs/bcm_host/include/ \
	-I /opt/vc/userland-master/interface/vmcs_host/linux/ \
	-I /opt/vc/userland-master/interface/vcos/pthreads/ \
	-I /opt/vc/userland-master/interface/vcos/ \
	\
	-L /opt/vc/userland-master/build/lib \
	\
	-lmmal_core \
	-lmmal \
	-lmmal_util \
	-lvcos \
	-lcontainers \
	-lbcm_host \
	-lmmal_vc_client \
	-lmmal_components \
	-lvchiq_arm \
	-lrt \
	\
	-g \
	\
	debug.c \
	callback.c \
	setup.c \
	mmal.c \
	main.c
