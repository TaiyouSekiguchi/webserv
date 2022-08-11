#include "Pipe.hpp"

Pipe::Pipe(void)
{
	OpenPipe();
	NonBlockingPipe();
}

Pipe::~Pipe()
{
}

void	Pipe::OpenPipe(void)
{
	if (pipe(pipe_) < 0)
		throw std::runtime_error("pipe error");
}

void	Pipe::NonBlockingPipe(void)
{
	int		val;

	val = fcntl(pipe_[READ], F_GETFL, 0);
	fcntl(pipe_[READ], F_SETFL, val | O_NONBLOCK);

	val = fcntl(pipe_[WRITE], F_GETFL, 0);
	fcntl(pipe_[WRITE], F_SETFL, val | O_NONBLOCK);
}

void	Pipe::CloseReadPipe(void)
{
	if (close(pipe_[READ]) < 0)
		throw std::runtime_error("close error");
}

void	Pipe::CloseWritePipe(void)
{
	if (close(pipe_[WRITE]) < 0)
		throw std::runtime_error("close error");
}

int		Pipe::WriteToPipe(void* buf, unsigned int byte)
{
	return (write(pipe_[WRITE], buf, byte));
}

ssize_t	Pipe::ReadFromPipe(void* buf, size_t byte)
{
	return (read(pipe_[READ], buf, byte));
}

int		Pipe::StdinRedirectToReadPipe(void)
{
	if (close(STDIN_FILENO) < 0
		|| dup2(pipe_[READ], STDIN_FILENO) < 0
		|| close(pipe_[READ]) < 0)
	{
		return (0);
	}

	return (1);
}

int		Pipe::StdoutRedirectToWritePipe(void)
{
	if (close(STDOUT_FILENO) < 0
		|| dup2(pipe_[WRITE], STDOUT_FILENO) < 0
		|| close(pipe_[WRITE]) < 0)
	{
		return (0);
	}

	return (1);
}
