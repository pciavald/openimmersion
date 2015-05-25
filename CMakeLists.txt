cmake_minimum_required (VERSION 2.6)
project (OpenImmersion)
set (Tutorial_VERSION_MAJOR 0)
set (Tutorial_VERSION_MINOR 1)

add_definitions(
	-DSTANDALONE
	-D__STDC_CONSTANT_MACROS
	-D__STDC_LIMIT_MACROS
	-DTARGET_POSIX
	-D_LINUX
	-DPIC
	-D_REENTRANT
	-D_LARGEFILE64_SOURCE
	-D_FILE_OFFSET_BITS=64
	-DHAVE_LIBOPENMAX=2
	-DOMX
	-DOMX_SKIP64BIT
	-DUSE_EXTERNAL_OMX
	-DHAVE_LIBBCM_HOST
	-DUSE_EXTERNAL_LIBBCM_HOST
	-DUSE_VCHIQ_ARM
	)

set(CMAKE_CXX_FLAGS "
	-U_FORTIFY_SOURCE
	-ftree-vectorize
	-pipe
	-Wall
	-Werror
	-O2
	")

include_directories(
	"${PROJECT_BINARY_DIR}"
	"/opt/vc/include"
	"/opt/vc/include/interface/vcos/pthreads"
	"/opt/vc/include/interface/vmcs_host/linux"
	)

link_directories(
	"/opt/vc/lib"
	)

add_executable(get_image get_image.c)

target_link_libraries(get_image
	openmaxil
	vcos
	vchiq_arm
	bcm_host
	pthread
	)
