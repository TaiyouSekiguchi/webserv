#include <unistd.h>
#include "AIo.hpp"

AIo::AIo(const int fd)
	: fd_(fd)
{
}

AIo::~AIo()
{
	close(fd_);
}

int		AIo::GetFd() const { return (fd_); }
