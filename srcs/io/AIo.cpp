#include <unistd.h>
#include <fcntl.h>
#include "AIo.hpp"

AIo::AIo(const int fd)
	: fd_(fd)
{
	if (fd_ != -1)
	{
		int val = fcntl(fd_, F_GETFL, 0);
		fcntl(fd_, F_SETFL, val | O_NONBLOCK);
	}
}

AIo::~AIo()
{
	if (fd_ != -1)
		close(fd_);
}

int		AIo::GetFd() const { return (fd_); }
