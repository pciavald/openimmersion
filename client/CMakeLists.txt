cmake_minimum_required	(VERSION 2.6)
project					(OpenImmersion)
set						(VERSION_MAJOR 0)
set						(VERSION_MINOR 1)

set(CMAKE_CXX_FLAGS "
	-Wall
	-Wextra
	-Werror
	")

include_directories(
	"${CMAKE_CURRENT_SOURCE_DIR}/inc"
	"/opt/vc/userland-master/"
	"/opt/vc/userland-master/build/inc/interface/vcos/"
	"/opt/vc/userland-master/host_applications/linux/apps/raspicam/"
	"/opt/vc/userland-master/host_applications/linux/libs/bcm_host/include/"
	"/opt/vc/userland-master/interface/vmcs_host/linux/"
	"/opt/vc/userland-master/interface/vcos/pthreads/"
	"/opt/vc/userland-master/interface/vcos/"
	)

link_directories(
	"/opt/vc/userland-master/build/lib"
	)

add_executable(			client
	src/debug.c
	src/callback.c
	src/setup.c
	src/init.c
	src/mmal.c
	src/client.c
	src/main.c
	)

target_link_libraries(	client
	bcm_host
	mmal
	mmal_core
	mmal_util
	mmal_components
	vcos
	containers
	vchiq_arm
	rt
	)
