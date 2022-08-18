#include "CGI.hpp"

CGI::CGI(const URI& uri, const HTTPRequest& req)
	: uri_(uri)
	, req_(req)
	, multiple_location_(false)
	, status_code_(SC_OK)
	, status_flag_(false)
{
}

CGI::~CGI(void)
{
}

e_HTTPServerEventType	CGI::ExecCGI(void)
{
	if (to_cgi_pipe_.Fail() || from_cgi_pipe_.Fail())
		throw HTTPError(SC_BAD_GATEWAY, "ExecCGI");

	if ((pid_ = fork()) < 0)
		throw HTTPError(SC_BAD_GATEWAY, "ExecCGI");

	if (pid_ == 0)
		ExecveCGIScript();
	else
	{
		to_cgi_pipe_.ChangeNonBlocking(Pipe::WRITE);
		from_cgi_pipe_.ChangeNonBlocking(Pipe::READ);

		if (to_cgi_pipe_.CloseFd(Pipe::READ) < 0
			|| from_cgi_pipe_.CloseFd(Pipe::WRITE) < 0)
		{
			throw HTTPError(SC_BAD_GATEWAY, "ExecCGI");
		}
	}

	if (req_.GetMethod() == "POST")
		return (SEVENT_CGI_WRITE);
	else
		return (SEVENT_CGI_READ);
}

void	CGI::PostToCgi(void)
{
	if (req_.GetMethod() == "POST")
	{
		if (to_cgi_pipe_.WriteToPipe(req_.GetBody()) < 0
			|| to_cgi_pipe_.CloseFd(Pipe::WRITE) < 0)
		throw HTTPError(SC_BAD_GATEWAY, "PostToCGI");
	}
}

e_HTTPServerEventType	CGI::ReceiveCgiResult(void)
{
	std::string		tmp;
	ssize_t			read_byte;
	pid_t			ret_pid;
	int				status;

	read_byte = from_cgi_pipe_.ReadFromPipe(&tmp);
	if (read_byte == -1)
		throw HTTPError(SC_BAD_GATEWAY, "ReceiveCgiResult");
	else if (read_byte != 0)
	{
		data_.append(tmp);
		return (SEVENT_CGI_READ);
	}

	if (from_cgi_pipe_.CloseFd(Pipe::READ) < 0)
		throw HTTPError(SC_BAD_GATEWAY, "ReceiveCgiResult");

	ret_pid = waitpid(pid_, &status, 0);
	if (ret_pid < 0
		|| !WIFEXITED(status)
		|| WEXITSTATUS(status) == EXIT_FAILURE)
	{
		headers_["content-type"] = "text/plain";
		status_code_ = SC_BAD_GATEWAY;
		body_ = "502 Bad Gateway";
		return (SEVENT_NO);
	}

	if (data_.empty())
	{
		headers_["content-type"] = "text/plain";
		status_code_ = SC_BAD_GATEWAY;
		body_ = "An error occurred while reading CGI reply (no response received)";
	}
	else
		ParseCGI();

	return (SEVENT_NO);
}

void	CGI::ExecveCGIScript(void)
{
	CGIEnv	env(uri_, req_);
	char*	argv[2];

	if (to_cgi_pipe_.CloseFd(Pipe::WRITE) < 0
		|| to_cgi_pipe_.RedirectToPipe(Pipe::READ, STDIN_FILENO) < 0
		|| from_cgi_pipe_.CloseFd(Pipe::READ) < 0
		|| from_cgi_pipe_.RedirectToPipe(Pipe::WRITE, STDOUT_FILENO) < 0)
		std::exit(EXIT_FAILURE);

	argv[0] = const_cast<char *>(uri_.GetAccessPath().c_str());
	argv[1] = NULL;

	if (execve(argv[0], argv, env.GetEnv()) < 0)
	{
		std::cerr << "execve failed." << std::endl;
		std::exit(EXIT_FAILURE);
	}
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
			throw HTTPError(SC_BAD_GATEWAY, "ParseCGI");

		line = data_.substr(offset, pos - offset);
		offset = pos + 1;
		if (line == "")
			break;
		ParseHeader(line);
		if (multiple_location_ == true)
			break;
	}

	if ((headers_.empty() && status_flag_ == false)
		|| multiple_location_ == true)
	{
		headers_.clear();
		status_code_ = SC_BAD_GATEWAY;
		body_ = std::string("An error occurred while parsing CGI reply");
	}
	else
		body_ = data_.substr(offset);
}

void	CGI::ParseHeader(const std::string& line)
{
	std::string::size_type	pos;
	std::string				field;
	std::string				content;

	pos = line.find(":");
	if (pos == std::string::npos)
		return ;

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

void	CGI::ParseContentType(const std::string& content)
{
	if (content != "")
		headers_["content-type"] = Utils::MyTrim(content, " ");
}

void	CGI::ParseLocation(const std::string& content)
{
	if (headers_.count("location") > 0)
		multiple_location_ = true;
	else
	{
		headers_["location"] = Utils::MyTrim(content, " ");
		if (status_flag_ == false)
			status_code_ = SC_FOUND;
	}
}

void	CGI::ParseStatusCode(const std::string& content)
{
	long	status_code;
	char*	endptr;

	status_code =  std::strtol(content.c_str(), &endptr, 10);
	if (*endptr != '\0' || errno == ERANGE || status_code < 1 || 999 < status_code)
		throw HTTPError(SC_BAD_GATEWAY, "ParseStatusCode");

	if (status_flag_ == false)
	{
		status_code_ = static_cast<e_StatusCode>(status_code);
		status_flag_ = true;
	}
}

std::string							CGI::GetData(void) const { return (data_); }
std::map<std::string, std::string>	CGI::GetHeaders(void) const { return (headers_); }
e_StatusCode						CGI::GetStatusCode(void) const { return (status_code_); }
std::string							CGI::GetBody(void) const { return (body_); }
int									CGI::GetToCgiWriteFd(void) const { return (to_cgi_pipe_.GetPipeFd(Pipe::WRITE)); }
int									CGI::GetFromCgiReadFd(void) const { return (from_cgi_pipe_.GetPipeFd(Pipe::READ)); }
