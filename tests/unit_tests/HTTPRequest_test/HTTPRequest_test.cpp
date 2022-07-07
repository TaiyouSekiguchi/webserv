#include <gtest/gtest.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "ListenSocket.hpp"
#include "./HTTPRequest.hpp"

int		CreateSocket(void)
{
	int		sock;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		throw "socket failed.";

	return (sock);
}

int		DummyClientConnect(void)
{
	int					sock;
	struct sockaddr_in	a_addr;

	sock = CreateSocket();

	memset(&a_addr, 0, sizeof(struct sockaddr_in));
	a_addr.sin_family = AF_INET;
	a_addr.sin_port = htons((unsigned short)8080);
	a_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (connect(sock, (struct sockaddr*)&a_addr, sizeof(struct sockaddr_in)) == -1)
	{
		close(sock);
		throw "connect failed.";
	}

	return (sock);
}

void	SendTestData(int sock, const char *msg)
{
	int				send_size;

	send_size = send(sock, msg, strlen(msg) + 1, 0);
	if (send_size == -1)
		throw "send failed.";

	return ;
}

ServerSocket	*CreateServerSocket(const char *msg)
{
	ListenSocket	*lsocket = new ListenSocket();
	lsocket->ListenConnection();

	int	client_fd;
	client_fd = DummyClientConnect();

	int	server_fd;
	server_fd = lsocket->AcceptConnection();

	ServerSocket	*server_socket = new ServerSocket(server_fd);

	SendTestData(client_fd, msg);
	close(client_fd);

	return (server_socket);
}

TEST(HTTPRequestTest1, SimpleRequest)
{
	const char*		msg = "GET / HTTP/1.0\r\n\r\n";
	ServerSocket	*ssocket = CreateServerSocket(msg);

	HTTPRequest		req;
	req.ParseRequest(*ssocket);

	EXPECT_EQ(HTTPRequest::GET, req.GetMethod());
	EXPECT_EQ("/", req.GetTarget());
	EXPECT_EQ("HTTP/1.0", req.GetVersion());

	delete ssocket;
}

TEST(HTTPRequestTest2, SimpleRequest)
{
	const char*		msg = "GET / HTTP/1.0\r\n\r\n";
	ServerSocket	*ssocket = CreateServerSocket(msg);

	HTTPRequest		req;
	req.ParseRequest(*ssocket);

	EXPECT_EQ(HTTPRequest::GET, req.GetMethod());
	EXPECT_EQ("/", req.GetTarget());
	EXPECT_EQ("HTTP/1.0", req.GetVersion());

	delete ssocket;
}
