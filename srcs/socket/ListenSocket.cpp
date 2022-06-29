#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <stdexcept>
#include "ListenSocket.hpp"

ListenSocket::ListenSocket(const int port) :
	ASocket(-1)
{
	fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_ == -1)
		throw std::runtime_error("socket error");

	int		sock_optval = 1;
	if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &sock_optval, sizeof(sock_optval)) == -1)
		throw std::runtime_error("setsockopt error");

	struct sockaddr_in	addr_info;
	memset(&addr_info, 0, sizeof(struct sockaddr_in));
	addr_info.sin_family = AF_INET;
	addr_info.sin_port = htons(port);
	addr_info.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(fd_, (const struct sockaddr *)&addr_info, sizeof(addr_info)) == -1)
		throw std::runtime_error("bind error");
}

ListenSocket::~ListenSocket()
{
}

void	ListenSocket::ListenConnection(const int backlog) const
{
	if (listen(fd_, backlog) == -1)
		throw std::runtime_error("listen error");
	std::cout << "fd : " << fd_ << " Listen!!" << std::endl;
}

int		ListenSocket::AcceptConnection() const
{
	int sockfd = accept(fd_, NULL, NULL);
	if (sockfd == -1)
		throw std::runtime_error("accept error");
	return (sockfd);
}
