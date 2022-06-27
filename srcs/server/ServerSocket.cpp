#include <unistd.h>
#include <sys/socket.h>
#include "ServerSocket.hpp"

ServerSocket::ServerSocket(const int fd) :
	fd_(fd)
{
}

ServerSocket::~ServerSocket()
{
	close(fd_);
}

std::string ServerSocket::RecvRequest() const
{
	const ssize_t	kSize = 1048576;     // 1MiB バイト
    char 			request_msg[kSize];

	int recv_size = recv(fd_, request_msg, kSize, 0);
    if (recv_size == -1)
		throw std::runtime_error("recv error");
	return (std::string(request_msg));
}

void	ServerSocket::SendResponse(const std::string& response_msg) const
{
    int send_size = send(fd_, response_msg.c_str(), response_msg.size(), 0);
	if (send_size == -1)
		throw std::runtime_error("send error");
}

