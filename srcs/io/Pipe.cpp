#include "Pipe.hpp"

Pipe::Pipe(void)
{
	pipe_[READ] = -1;
	pipe_[WRITE] = -1;
}

Pipe::~Pipe(void)
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

int		Pipe::ClosePipe(e_PipeIo type)
{
	int		result = 0;

	if (pipe_[type] != -1)
	{
		result = close(pipe_[type]);
		pipe_[type] = -1;
	}

	return (result);
}

void	Pipe::NonBlockingPipe(e_PipeIo type) const
{
	int		val;

	val = fcntl(pipe_[type], F_GETFL, 0);
	fcntl(pipe_[type], F_SETFL, val | O_NONBLOCK);
}

int		Pipe::WriteToPipe(const std::string& str) const
{
	return (write(pipe_[WRITE], str.c_str(), str.size()));
}

ssize_t	Pipe::ReadFromPipe(std::string* str) const
{
	const int			buf_size = 8;
	char				buf[buf_size + 1];
	ssize_t				readsize;

	if((readsize = read(pipe_[READ], &buf, buf_size)) > 0)
	{
		buf[readsize] = '\0';
		*str += buf;
	}
	return (readsize);
}

int		Pipe::RedirectToPipe(e_PipeIo type, int fd)
{
	if (close(fd) < 0
		|| dup2(pipe_[type], fd) < 0
		|| ClosePipe(type) < 0)
	{
		return (0);
	}
	return (1);
}

int		Pipe::GetPipeFd(e_PipeIo type) const
{
	return (pipe_[type]);
}
