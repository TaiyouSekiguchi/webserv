#include "Pipe.hpp"

Pipe::Pipe(e_PipeIo type)
	: type_(type)
{
	pipe_[READ] = -1;
	pipe_[WRITE] = -1;
}

Pipe::~Pipe()
{
	if (pipe_[READ] != -1)
		close(pipe_[READ]);
	if (pipe_[WRITE] != -1)
		close(pipe_[WRITE]);
}

int		Pipe::OpenPipe(void)
{
	return (pipe(pipe_));
}

void	Pipe::NonBlockingPipe(void)
{
	if (type_ == WRITE)
	{
		val = fcntl(pipe_[WRITE], F_GETFL, 0);
		fcntl(pipe_[WRITE], F_SETFL, val | O_NONBLOCK);
	}
	else
	{
		val = fcntl(pipe_[READ], F_GETFL, 0);
		fcntl(pipe_[READ], F_SETFL, val | O_NONBLOCK);
	}
}

static int	MyClose(int* fd)
{
	int		result = 0;

	if (*fd != -1)
	{
		result = close(*fd);
		fd* = -1;
	}

	return (result);
}

int		Pipe::CloseUnusedPipeInParentProcess(void)
{
	int		result = 0;

	if (type_ == WRITE)
		result = MyClose(&pipe_[READ]);
	else
		result = MyClose(&pipe_[WRITE]);

	return (result);
}

int		Pipe::CloseUnusedPipeInChildProcess(void)
{
	int		result;

	if (type_ == WRITE)
		result = MyClose(&pipe_[WRITE]);
	else
		result = MyClose(&pipe_[READ]);

	return (result);
}

int		Pipe::WriteToPipe(void* buf, unsigned int byte) const
{
	return (write(pipe_[WRITE], buf, byte));
}

ssize_t	Pipe::ReadFromPipe(void* buf, size_t byte) const
{
	return (read(pipe_[READ], buf, byte));
}

int		Pipe::RedirectToPipe(void)
{
	int		result = 0;

	if (type_ == WRITE)
	{
		if (close(STDIN_FILENO) < 0
			|| dup2(pipe_[READ], STDIN_FILENO) < 0
			|| MyClose(&pipe_[READ]) < 0)
		{
			return (0);
		}
		return (1);
	}
	else
	{
		if (close(STDOUT_FILENO) < 0
			|| dup2(pipe_[WRITE], STDOUT_FILENO) < 0
			|| MyClose(&pipe_[WRITE]) < 0)
		{
			return (0);
		}
		return (1);
	}
}

int		Pipe::GetPipeFd(void) const
{
	if (type_ == WRITE)
		return (pipe_[WRITE]);
	else
		return (pipe_[READ]);
}
