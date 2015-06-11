#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "Socket.hpp"
#include "Ros.hpp"
#include <strings.h>

/*
void	sig_handler(int val)
{
	switch (val)
	{
		case SIGPIPE:
			fprintf(stderr, "broken pipe\n");
			break ;
		case SIGINT:
			fprintf(stderr, "interrupt\n");
			break ;
	}
	exit(1);
}

void	init(int argc, char ** argv)
{
	signal(SIGPIPE, sig_handler);
	signal(SIGINT, sig_handler);
	ros::init(argc, argv, "server");
}
*/
void	dump(Socket & s, int c)
{
	int		i = 0;
	int		n;
	char	buff[BUFF_SIZE];

	bzero(buff, BUFF_SIZE);
	while ((n = read(s.getClients()[0], buff, BUFF_SIZE)) > 0)
	{
		if (n < 0) throw (string("could not read from socket"));
		if (i++ == 300)
			write(1, buff, BUFF_SIZE);
		bzero(buff, BUFF_SIZE);
	}
}

static void		get(char ** buff, Socket & sock)
{
	int			n;

	for (int i = 0; i < MAX_PEERS; i++)
	{
		cerr << sock.getClients()[0] << endl;
		cerr << buff[i] << endl;
		n = read(sock.getClients()[i], buff[i], BUFF_SIZE);
		if (n != BUFF_SIZE)
			throw (string("could not read from socket"));
	}
}

static void		compute(geometry_msgs::Point & pose, char ** buff)
{
	/*
	 * triangulation algorithm happends here
	 */

	// test
	pose.x = 1;
	pose.y = 2;
	pose.z = 3;
}

static void		clean(char ** buff)
{
	for (int i = 0; i < MAX_PEERS; i++)
		bzero(buff[i], BUFF_SIZE);
}

int		sockfd;
int		newsockfd;
void	sig_handler(int val)
{
	switch (val)
	{
		case SIGPIPE:
			fprintf(stderr, "broken pipe\n");
			break ;
		case SIGINT:
			fprintf(stderr, "interrupt\n");
			break ;
	}
	close(newsockfd);
	close(sockfd);
	exit(1);
}
void error(const char *msg)
{
	perror(msg);
	exit(1);
}
int		main(int argc, char ** argv)
{
	int				portno;
	socklen_t		clilen;
	char			buffer[BUFF_SIZE];
	t_sockaddr_in	serv_addr;
	t_sockaddr_in	cli_addr;
	int				n;

	signal(SIGPIPE, sig_handler);
	signal(SIGINT, sig_handler);

	if (argc < 2)
	{
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");

	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR on binding");

	listen(sockfd,5);

	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0) 
		error("ERROR on accept");

	int		stat = 0;
	bzero(buffer, BUFF_SIZE);
	while ((n = read(newsockfd, buffer, BUFF_SIZE)) > 0)
	{
		if (n < 0) error("ERROR reading from socket");
		printf("received %i\n", n);
		bzero(buffer, BUFF_SIZE);
	}

	//n = write(newsockfd,"I got your message",18);
	//if (n < 0) error("ERROR writing to socket");
	close(newsockfd);
	close(sockfd);
	return 0;

	/*
	try
	{
		init(argc, argv);
		Socket		s;
		Ros			ros;
		char		buff[MAX_PEERS][BUFF_SIZE];

		geometry_msgs::Point	pose;

		while (42)
		{
			clean((char **)buff);
			get((char **)buff, s);
			compute(pose, (char **)buff);
			ros.send(pose);
		}
	}
	catch (string & s) {
		cerr << s << endl;
	}
	return (0);
	*/
}
