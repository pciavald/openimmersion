cmake_minimum_required (VERSION 2.6)
project (OpenImmersion)
set (Tutorial_VERSION_MAJOR 0)
set (Tutorial_VERSION_MINOR 1)

set(CMAKE_CXX_FLAGS "
	-U_FORTIFY_SOURCE
	-ftree-vectorize
	-pipe
	-Wall
	-Wextra
	-Werror
	-O2
	")

include_directories(
	"${PROJECT_BINARY_DIR}"
	#"/opt/vc/include"
	#"/opt/vc/include/interface/vcos/pthreads"
	#"/opt/vc/include/interface/vmcs_host/linux"

	"/opt/vc/userland-master/"
	#"/opt/vc/userland-master/build/inc/interface/vcos/"
	#"/opt/vc/userland-master/host_applications/linux/apps/raspicam/"
	"/opt/vc/userland-master/host_applications/linux/libs/bcm_host/include/"
	#"/opt/vc/userland-master/interface/vmcs_host/linux/"
	#"/opt/vc/userland-master/interface/vcos/pthreads/"
	#"/opt/vc/userland-master/interface/vcos/"
	)

link_directories(
	#"/opt/vc/lib"
	"/opt/vc/userland-master/build/lib"
	)

add_executable(get_image clean.c)

target_link_libraries(get_image
	openmaxil
	vcos
	vchiq_arm
	bcm_host
	pthread
	)
