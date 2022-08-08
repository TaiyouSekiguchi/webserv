#include <unistd.h>
#include "AIo.hpp"

AIo::AIo(const int fd)
	: fd_(fd)
{
}

AIo::~AIo()
{
	if (fd_ != -1)
		close(fd_);
}

int		AIo::GetFd() const { return (fd_); }
