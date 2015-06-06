#include "openimmersion.h"

int		main(void)
{
	init();
	init_camera();
	init_preview();
	init_client(SERVER_NAME, SERVER_PORT);
	start_capture();
	while (42)
		;
	return (0);
}
