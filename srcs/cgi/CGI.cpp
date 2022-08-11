#include "CGI.hpp"

CGI::CGI(const URI& uri, const HTTPRequest& req)
	: uri_(uri)
	, req_(req)
	, server_conf_(req.GetServerConf())
	, status_code_(SC_OK)
	, write_pipe_(WRITE)
	, read_pipe_(READ)
{
}

CGI::~CGI(void)
{
}

void	CGI::ExecveCGIScript(void)
{
	CGIEnv	env(uri_, req_);
	char*	argv[2];

	if (write_pipe_.CloseUnusedPipeInChildProcess() < 0
		|| write_pipe_.RedirectToPipe() < 0
		|| read_pipe_.CloseUnusedPipeInChildProcess() < 0
		|| read_pipe_.RedirectToPipe() < 0)
		std::exit(EXIT_FAILURE);

	argv[0] = const_cast<char *>(uri_.GetAccessPath().c_str());
	argv[1] = NULL;

	if (execve(argv[0], argv, env.GetEnv()) < 0)
		std::exit(EXIT_FAILURE);
}

/*
void	CGI::ReceiveData(pid_t pid)
{
	const size_t	buf_size = 4;
	char			buf[buf_size + 1];
	int				read_byte;
	pid_t			ret_pid;
	int				status;

	if (write_pipe_.CloseUnusedPipeInParentProcess() < 0
		|| read_pipe_.CloseUnusedPipeInParentProcess() < 0)
		throw HTTPError(SC_INTERNAL_SERVER_ERROR, "ReceiveData");


	//PostToCgi()
	if (req_.GetMethod() == "POST")
	{
		if (write_pipe_.WriteToPipe(req_.GetBody().c_str(), req_.GetBody().size()) < 0)
			throw HTTPError(SC_INTERNAL_SERVER_ERROR, "ReceiveData");
	}

	while (1)
	{
		read_byte = read_pipe_.ReadFromPipe(buf, buf_size);
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
*/

e_HTTPServerEventType	CGI::ExecuteCGI(void)
{
	if (write_pipe_.OpenPipe() < 0
		|| read_pipe_.OpenPipe() < 0)
		throw HTTPError(SC_INTERNAL_SERVER_ERROR, "ExecuteCGI");

	if ((pid_ = fork()) < 0)
		throw HTTPError(SC_INTERNAL_SERVER_ERROR, "ExecuteCGI");

	if (pid_ == 0)
		ExecveCGIScript();
	else
	{
		write_pipe_.NonBlockingPipe();
		read_pipe_.NonBlockingPipe();

		if (write_pipe_.CloseUnusedPipeInParentProcess() < 0
			|| read_pipe_.CloseUnusedPipeInParentProcess() < 0)
			throw HTTPError(SC_INTERNAL_SERVER_ERROR, "ReceiveData");

		if (req_.GetMethod() == "POST")
			return (SEVENT_CGI_WRITE);
		else
			return (SEVENT_CGI_READ);
	}
}

void	CGI::PostToCGI(void)
{
	if (req_.GetMethod() == "POST")
	{
		if (write_pipe_.WriteToPipe(req_.GetBody().c_str(), req_.GetBody().size()) < 0)
			throw HTTPError(SC_INTERNAL_SERVER_ERROR, "PostToCGI");
	}
}

e_HTTPServerEventType	CGI::ReceiveCgiResult(const bool eof_flag)
{
	const size_t	buf_size = 4;
	char			buf[buf_size + 1];
	int				read_byte;
	pid_t			ret_pid;
	int				status;

	if (!eof_flag)
	{
		read_byte = read_pipe_.ReadFromPipe(buf, buf_size);
		buf[read_byte] = '\0';
		data_ += std::string(buf);

		return (SEVENT_CGI_READ);
	}
	else
	{
		ret_pid = waitpid(pid_, &status, 0);
		if (ret_pid < 0
			|| !WIFEXITED(status)
			|| WEXITSTATUS(status) == EXIT_FAILURE)
			throw HTTPError(SC_INTERNAL_SERVER_ERROR, "ReceiveCgiResult");

		ParseCGI();

		return (SEVENT_NO);
	}
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
int				CGI::GetCgiWriteFd(void) const { return (write_pipe_.GetPipeFd()); }
int				CGI::GetCgiReadFd(void) const { return (read_pipe_.GetPipeFd()); }
