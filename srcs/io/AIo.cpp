#include <unistd.h>
#include <fcntl.h>
#include "AIo.hpp"

AIo::AIo(const int fd)
	: fd_(fd)
{
	if (fd_ == -1)
	{
		int val = fcntl(sockfd, F_GETFL, 0);
		fcntl(sockfd, F_SETFL, val | O_NONBLOCK);
	}
}

AIo::~AIo()
{
	if (fd_ != -1)
		close(fd_);
}

int		AIo::GetFd() const { return (fd_); }
