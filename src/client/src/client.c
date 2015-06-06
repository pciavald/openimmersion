#include "openimmersion.h"
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

typedef struct sockaddr_in		t_sockaddr_in;
typedef struct sockaddr			t_sockaddr;
typedef struct addrinfo			t_addrinfo;
typedef struct in_addr			t_in_addr;

static void		wait_for_server(int fd, t_sockaddr * s_addr)
{
	int		ret = -1;

	fprintf(stderr, "waiting for server... ");
	while (ret != 0)
		ret = connect(fd, s_addr, sizeof (t_sockaddr_in));
	fprintf(stderr, "connected.\n");
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
	check(g_status, __func__, __LINE__, "resolving host");

	inet_ntop(res->ai_family, res->ai_addr->sa_data, addrstr, 100);
	addr->sin_addr = ((t_sockaddr_in *)res->ai_addr)->sin_addr;

	inet_ntop (res->ai_family, &(addr->sin_addr), addrstr, 100);
	printf ("server address: %s:%i (%s)\n", addrstr, port, res->ai_canonname);
}

void	init_client(char * name, int port)
{
	int				sock;
	t_sockaddr_in	sin = {0, 0, {0}, {0}};

	sock = socket(AF_INET, SOCK_STREAM, 0);
	check(sock < 0 ? -1 : 0, __func__, __LINE__, "opening socket");

	lookup_host(&sin, name, port);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);

	wait_for_server(sock, (t_sockaddr *)&sin);
	g_data.server = sock;
}
