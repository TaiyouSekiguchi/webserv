#include <unistd.h>
#include <sys/socket.h>
#include "ServerSocket.hpp"

ServerSocket::ServerSocket(const int fd) :
	ASocket(fd), socket_status_(CONNECT)
{
}

ServerSocket::~ServerSocket()
{
}

std::string ServerSocket::RecvData(const ssize_t size) const
{
	char 	buf[size + 1];

	ssize_t recv_size = recv(fd_, buf, size, 0);
	if (recv_size == -1)
		throw std::runtime_error("recv error");
	buf[recv_size] = '\0';
	return (std::string(buf));
}

void	ServerSocket::SendData(const std::string& data) const
{
	ssize_t send_size = send(fd_, data.c_str(), data.size(), 0);
	if (send_size == -1)
		throw std::runtime_error("send error");
}

void	ServerSocket::DisconnectSocket(void)
{
	socket_status_ = ServerSocket::DISCONNECT;
}

ServerSocket::e_status	ServerSocket::GetSocketStatus(void) const
{
	return (socket_status_);
}
