#include <unistd.h>
#include <sys/socket.h>
#include "ServerSocket.hpp"

ServerSocket::ServerSocket(const int fd) :
	ASocket(fd)
{
}

ServerSocket::~ServerSocket()
{
}

std::string ServerSocket::RecvMsg(const ssize_t size) const
{
	char 	msg[size + 1];

	ssize_t recv_size = recv(fd_, msg, size, 0);
	if (recv_size == -1)
		throw std::runtime_error("recv error");
	msg[recv_size] = '\0';
	return (std::string(msg));
}

void	ServerSocket::SendMsg(const std::string& msg) const
{
	ssize_t send_size = send(fd_, msg.c_str(), msg.size(), 0);
	if (send_size == -1)
		throw std::runtime_error("send error");
}

