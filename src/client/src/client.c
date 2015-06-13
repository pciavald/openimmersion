#include "openimmersion.h"

void			wait_for_server(t_sockaddr * s_addr)
{
	int		ret = -1;

	fprintf(stderr, "waiting for server... ");
	while (ret != 0)
		ret = connect(g_data.send, s_addr, sizeof (t_sockaddr_in));
	fprintf(stderr, "connected.\n");
	errno = 0;
}

static void		lookup_host(t_sockaddr_in * addr, char * name, int port)
{
	t_addrinfo		hints			= {0, 0, 0, 0, 0, 0, 0, 0};
	t_addrinfo *	res				= NULL;
	char			addrstr[100] 	= {0};

	hints.ai_family		= AF_INET;
	hints.ai_socktype	= SOCK_STREAM;
	hints.ai_flags		|= AI_CANONNAME;

	g_status = getaddrinfo(name, NULL, &hints, &res);
	if (g_status) fprintf(stderr, "%s\n", gai_strerror(g_status));
	check(g_status, __func__, __LINE__, "resolving host");

	inet_ntop(res->ai_family, res->ai_addr->sa_data, addrstr, 100);
	addr->sin_addr = ((t_sockaddr_in *)res->ai_addr)->sin_addr;

	inet_ntop (res->ai_family, &(addr->sin_addr), addrstr, 100);
	fprintf(stderr, "server address: %s:%i (%s)\n", addrstr, port, res->ai_canonname);
}

t_sockaddr_in	init_client(char * name, int port)
{
	t_sockaddr_in	sin = {0, 0, {0}, {0}};

	g_data.send = socket(AF_INET, SOCK_STREAM, 0);
	check(g_data.send < 2 ? -1 : 0, __func__, __LINE__, "opening sender");

	lookup_host(&sin, name, port);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);

	return (sin);
}
