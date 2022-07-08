#include <unistd.h>
#include "ASocket.hpp"

ASocket::ASocket(const int fd, const ServerDirective& server_conf)
	: fd_(fd), server_conf_(server_conf)
{
}

ASocket::~ASocket()
{
	close(fd_);
}

int		ASocket::GetFd() const
{
	return (fd_);
}

const ServerDirective&	ASocket::GetServerConf() const
{
	return (server_conf_);
}
