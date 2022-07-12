#include "CGI.hpp"

CGI::CGI(void)
{
}

CGI::~CGI(void)
{
}

static void	pipe_set(int src, int dst, int not_use)
{
	if (close(not_use) < 0
		|| close(dst) < 0
		|| dup2(src, dst) < 0
		|| close(src) < 0)
		throw HTTPError(HTTPError::INTERNAL_SERVER_ERROR);
}

void	CGI::do_child(const std::string& file_path, const int pipe_fd[2])
{
	char*	argv[2];
	char**	env;

	pipe_set(pipe_fd[1], 1, pipe_fd[0]);

	argv[0] = const_cast<char *>(file_path.c_str());
	argv[1] = NULL;
	env = NULL;

	if (execve(argv[0], argv, env) < 0)
		throw HTTPError(HTTPError::INTERNAL_SERVER_ERROR);
}

void	CGI::do_parent(const int pipe_fd[2])
{
	const size_t	buf_size = 4;
	char			buf[buf_size + 1];
	int				read_byte;
	int				status;

	pipe_set(pipe_fd[0], 0, pipe_fd[1]);

	while (1)
	{
		read_byte = read(0, buf, buf_size);
		if (read_byte < 0)
			throw HTTPError(HTTPError::INTERNAL_SERVER_ERROR);
		if (read_byte == 0)
			break;
		buf[buf_size] = '\0';
		data_ += std::string(buf);
	}

	if (wait(&status) < 0)
		throw HTTPError(HTTPError::INTERNAL_SERVER_ERROR);
}

void	CGI::ExecuteCGI(const std::string& file_path)
{
	int		pipe_fd[2];
	int		ret;

	if (pipe(pipe_fd) < 0)
		throw HTTPError(HTTPError::INTERNAL_SERVER_ERROR);

	if ((ret = fork()) < 0)
		throw HTTPError(HTTPError::INTERNAL_SERVER_ERROR);

	if (ret == 0)
		do_child(file_path, pipe_fd);
	else
		do_parent(pipe_fd);

	return;
}

void	CGI::ParseCGI(void)
{
}

std::string		CGI::GetData(void) const { return (data_); }
std::string		CGI::GetContentType(void) const { return (content_type_); }
std::string		CGI::GetBody(void) const { return (body_); }
