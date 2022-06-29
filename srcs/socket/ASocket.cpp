#include <unistd.h>
#include "ASocket.hpp"

ASocket::ASocket(const int fd) :
	fd_(fd)
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
