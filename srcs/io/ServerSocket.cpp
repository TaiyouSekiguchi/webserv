#include <unistd.h>
#include <sys/socket.h>
#include "ServerSocket.hpp"

ServerSocket::ServerSocket(const ListenSocket& lsocket)
	: ASocket(lsocket.AcceptConnection(), lsocket.GetListen(), lsocket.GetServerConfs())
{
}

ServerSocket::~ServerSocket()
{
}

ssize_t ServerSocket::RecvData(std::string* data, const ssize_t size) const
{
	if (failed_)
		throw std::runtime_error("ServerScocket error");

	char 	buf[size + 1];

	ssize_t recv_size = recv(fd_, buf, size, 0);
	if (recv_size <= 0)
		return (recv_size);
	buf[recv_size] = '\0';
	*data = buf;
	return (recv_size);
}

ssize_t	ServerSocket::SendData(const std::string& data) const
{
	if (failed_)
		throw std::runtime_error("ServerScocket error");

	ssize_t send_size = send(fd_, data.c_str(), data.size(), 0);
	return (send_size);
}
