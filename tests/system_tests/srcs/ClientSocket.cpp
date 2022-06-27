#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <stdexcept>
#include <unistd.h>
#include <string>
#include "ClientSocket.hpp"

ClientSocket::ClientSocket()
{
	fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_ == -1)
		throw std::runtime_error("socket error");
}

ClientSocket::~ClientSocket()
{
	close(fd_);
}

void	ClientSocket::ConnectServer(const char *ip, const int port) const
{
	struct sockaddr_in	addr_info;
    memset(&addr_info, 0, sizeof(struct sockaddr_in));
    addr_info.sin_family = AF_INET;
    addr_info.sin_port = htons(port);
    addr_info.sin_addr.s_addr = inet_addr(ip);
	const socklen_t	size = sizeof(struct sockaddr_in);
    if (connect(fd_, (struct sockaddr *)&addr_info, size) == -1)
		throw std::runtime_error("connect error");
}

void	ClientSocket::SendRequest(const std::string& request_msg) const
{
	if (send(fd_, request_msg.c_str(), request_msg.size(), 0) == -1)
		throw std::runtime_error("send error");
}

std::string	ClientSocket::RecvResponse() const
{
	const size_t	kSize = 4096;
    char			response_msg[kSize + 1];

	 ssize_t recv_size = recv(fd_, response_msg, kSize, 0);
    if (recv_size == -1)
		throw std::runtime_error("recv error");
	response_msg[recv_size] = '\0';
	return (std::string(response_msg));
}
