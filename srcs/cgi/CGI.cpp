#include "CGI.hpp"

CGI::CGI(const URI& uri, const HTTPRequest& req)
	: uri_(uri)
	, req_(req)
	, server_conf_(req.GetServerConf())
	, status_code_(SC_OK)
{
	ExecuteCGI();
	ParseCGI();
}

CGI::~CGI(void)
{
}

static int	pipe_set(int src, int dst)
{
	if (close(dst) < 0
		|| dup2(src, dst) < 0
		|| close(src) < 0)
	{
		return (0);
	}

	return (1);
}

void	CGI::SendData(int write_pipe_fd[2], int read_pipe_fd[2])
{
	CGIEnv	env(uri_, req_);
	char*	argv[2];

	if (req_.GetMethod() == "POST")
	{
		if (close(write_pipe_fd[1]) < 0 || !pipe_set(write_pipe_fd[0], STDIN_FILENO))
			std::exit(EXIT_FAILURE);
	}

	if (close(read_pipe_fd[0]) < 0 || !pipe_set(read_pipe_fd[1], STDOUT_FILENO))
		std::exit(EXIT_FAILURE);

	argv[0] = const_cast<char *>(uri_.GetAccessPath().c_str());
	argv[1] = NULL;

	if (execve(argv[0], argv, env.GetEnv()) < 0)
		std::exit(EXIT_FAILURE);
}

void	CGI::ReceiveData(int write_pipe_fd[2], int read_pipe_fd[2], pid_t pid)
{
	const size_t	buf_size = 4;
	char			buf[buf_size + 1];
	int				read_byte;
	pid_t			ret_pid;
	int				status;

	if (req_.GetMethod() == "POST")
	{
		if (close(write_pipe_fd[0]) < 0
			|| write(write_pipe_fd[1], req_.GetBody().c_str(), req_.GetBody().size()) < 0)
		throw HTTPError(SC_INTERNAL_SERVER_ERROR, "ReceiveData");
	}

	if (close(read_pipe_fd[1]) < 0)
		throw HTTPError(SC_INTERNAL_SERVER_ERROR, "ReceiveData");

	while (1)
	{
		read_byte = read(read_pipe_fd[0], buf, buf_size);
		if (read_byte < 0)
			throw HTTPError(SC_INTERNAL_SERVER_ERROR, "ReceiveData");
		if (read_byte == 0)
			break;
		buf[read_byte] = '\0';
		data_ += std::string(buf);
	}

	ret_pid = waitpid(pid, &status, 0);
	if (ret_pid < 0
		|| !WIFEXITED(status)
		|| WEXITSTATUS(status) == EXIT_FAILURE)
		throw HTTPError(SC_INTERNAL_SERVER_ERROR, "ReceiveData");
}

void	CGI::ExecuteCGI(void)
{
	int		write_pipe_fd[2];
	int		read_pipe_fd[2];
	pid_t	pid;

	if (req_.GetMethod() == "POST")
	{
		if (pipe(write_pipe_fd) < 0)
			throw HTTPError(SC_INTERNAL_SERVER_ERROR, "ExecuteCGI");
	}

	if (pipe(read_pipe_fd) < 0
		|| (pid = fork()) < 0)
		throw HTTPError(SC_INTERNAL_SERVER_ERROR, "ExecuteCGI");

	if (pid == 0)
		SendData(write_pipe_fd, read_pipe_fd);
	else
		ReceiveData(write_pipe_fd, read_pipe_fd, pid);

	if (req_.GetMethod() == "POST")
	{
		if (close(write_pipe_fd[1]) < 0)
			throw HTTPError(SC_INTERNAL_SERVER_ERROR, "ExecuteCGI");
	}

	if (close(read_pipe_fd[0]) < 0)
		throw HTTPError(SC_INTERNAL_SERVER_ERROR, "ExecuteCGI");

	return;
}

void	CGI::ParseContentType(const std::string& content)
{
	content_type_ = Utils::MyTrim(content, " ");
}

void	CGI::ParseLocation(const std::string& content)
{
	bool		is_url;

	is_url = content.find("http://") != std::string::npos;
	is_url |= content.find("https://") != std::string::npos;
	
	if (is_url)
	{
		location_ = Utils::MyTrim(content, " ");
		status_code_ = SC_FOUND;
	}
}

void	CGI::ParseStatusCode(const std::string& content)
{
	long	status_code;
	char*	endptr;

	status_code =  std::strtol(content.c_str(), &endptr, 10);
	if (*endptr != '\0' || errno == ERANGE || status_code < 1 || 999 < status_code)
		throw HTTPError(SC_INTERNAL_SERVER_ERROR, "ParseStatusCode");

	status_code_ = static_cast<e_StatusCode>(status_code);
}

void	CGI::ParseHeader(const std::string& line)
{
	std::string::size_type	pos;
	std::string				field;
	std::string				content;

	pos = line.find(":");
	if (pos == std::string::npos)
		throw HTTPError(SC_INTERNAL_SERVER_ERROR, "ParseHeader");

	field = line.substr(0, pos);
	content = line.substr(pos + 1);
	field = Utils::StringToLower(field);

	if (field == "content-type")
		ParseContentType(content);
	else if (field == "location")
		ParseLocation(content);
	else if (field == "status")
		ParseStatusCode(content);

	return;
}

void	CGI::ParseCGI(void)
{
	std::string::size_type	offset;
	std::string::size_type	pos;
	std::string				ret;
	std::string				line;

	offset = 0;
	while (1)
	{
		pos = data_.find("\n", offset);
		if (pos == std::string::npos)
			throw HTTPError(SC_INTERNAL_SERVER_ERROR, "ParseCGI");

		line = data_.substr(offset, pos - offset);
		offset = pos + 1;
		if (line == "")
			break;
		ParseHeader(line);
	}

	body_ = data_.substr(offset);
}

std::string		CGI::GetData(void) const { return (data_); }
std::string		CGI::GetContentType(void) const { return (content_type_); }
std::string		CGI::GetLocation(void) const { return (location_); }
e_StatusCode	CGI::GetStatusCode(void) const { return (status_code_); }
std::string		CGI::GetBody(void) const { return (body_); }
