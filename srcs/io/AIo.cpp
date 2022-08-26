#include <unistd.h>
#include <fcntl.h>
#include "AIo.hpp"

AIo::AIo(const int fd)
	: fd_(fd), failed_(false)
{
	if (fd_ == -1)
	{
		failed_ = true;
		return;
	}
	int val = fcntl(fd_, F_GETFL, 0);
	fcntl(fd_, F_SETFL, val | O_NONBLOCK);
}

AIo::~AIo()
{
	if (fd_ != -1)
		close(fd_);
}

bool	AIo::Fail() const { return (failed_); }
int		AIo::GetFd() const { return (fd_); }
