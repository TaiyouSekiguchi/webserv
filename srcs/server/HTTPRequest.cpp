#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest()
	: method_(NONE)
	, content_length_(0)
{
}

HTTPRequest::~HTTPRequest()
{
}

HTTPRequest::e_method	HTTPRequest::GetMethod(void) const
{
	return (method_);
}

std::string		HTTPRequest::GetTarget(void) const
{
	return (target_);
}

std::string		HTTPRequest::GetVersion(void) const
{
	return (version_);
}

std::string		HTTPRequest::GetHost(void) const
{
	return (host_);
}

size_t			HTTPRequest::GetContentLength(void) const
{
	return (content_length_);
}

std::string		HTTPRequest::GetBody(void) const
{
	return (body_);
}

std::string		HTTPRequest::GetLine(ServerSocket const & ssocket)
{
	std::string				data;
	std::string				line;
	std::string				separator;
	std::string::size_type	separator_length;
	std::string::size_type	pos;

	separator = "\r\n";
	separator_length = separator.length();

	while ((pos = save_.find(separator)) == std::string::npos)
	{
		data = ssocket.RecvData();
		if (data.size() == 0)
			throw ClientClosed();
		save_ += data;
	}

	line = save_.substr(0, pos);
	save_ = save_.substr(pos + separator_length, save_.size());

	return (line);
}

void	HTTPRequest::ParseMethod(std::string const & method)
{
	const std::string	methods[3] = { "GET", "POST", "DELETE" };
	const int			methods_size = 3;

	for (int i = 0; i < methods_size; i++)
	{
		if (method == methods[i])
			method_ = static_cast<HTTPRequest::e_method>(i);
	}
	if (method_ == NONE)
		throw HTTPError(HTTPError::BAD_REQUEST);
}

void	HTTPRequest::ParseTarget(std::string const & target)
{
	//const char		*cstring;
	//int				ret;

	/*
	cstring = target.c_str();
	ret = access(cstring, F_OK);
	if (ret == -1)
		throw HTTPError(HTTPError::NOT_FOUND);

	if (method_ == GET)
	{
		ret = access(cstring, R_OK);
		if (ret == -1)
			throw HTTPError(HTTPError::FORBIDDEN);
	}
	else if (method_ == POST)
	{
		ret = access(cstring, W_OK);
		if (ret == -1)
			throw HTTPError(HTTPError::FORBIDDEN);
	}
	*/
	if (target[0] != '/')
		throw HTTPError(HTTPError::BAD_REQUEST);

	target_ = target;
}

void	HTTPRequest::ParseVersion(std::string const & version)
{
	if (version == "HTTP/1.0" || version == "HTTP/1.1")
		version_ = version;
	else
		throw HTTPError(HTTPError::BAD_REQUEST);
}

void	HTTPRequest::ParseRequestLine(ServerSocket const & ssocket)
{
	std::string					line;
	std::vector<std::string>	list;

	while ((line = GetLine(ssocket)) == "")
		;

	list = Utils::MySplit(line, " ");
	if (list.size() != 3)
		throw HTTPError(HTTPError::BAD_REQUEST);

	ParseMethod(list.at(0));
	ParseTarget(list.at(1));
	ParseVersion(list.at(2));

	return ;
}

void HTTPRequest::ParseHost(const std::string& content)
{
	std::vector<std::string>	list;

	list = Utils::MySplit(content, " ");
	if (list.size() != 1)
		throw HTTPError(HTTPError::BAD_REQUEST);

	host_ = list.at(0);
}

void HTTPRequest::ParseContentLength(const std::string& content)
{
	std::vector<std::string>	list;
	std::string					tmp;
	const char					*c_content_length;
	char						*endptr;

	list = Utils::MySplit(content, " ");
	if (list.size() != 1)
		throw HTTPError(HTTPError::BAD_REQUEST);

	tmp = list.at(0);
	c_content_length = tmp.c_str();
	content_length_ = std::strtoul(c_content_length, &endptr, 10);
	if (errno == ERANGE)
		throw HTTPError(HTTPError::BAD_REQUEST);
	if (*endptr != '\0')
		throw HTTPError(HTTPError::BAD_REQUEST);
}

void HTTPRequest::ParseUserAgent(const std::string& content)
{
	user_agent_ = content;
}

void HTTPRequest::ParseAcceptEncoding(const std::string& content)
{
	accept_encoding_ = Utils::MySplit(content, " ");
}

void	HTTPRequest::ParseHeader(const std::string& field, const std::string& content)
{
	const size_t		size = 4;
	const std::string	fields[size] = {
		"Host:",
		"Content-Length:",
		"User-Agent:",
		"Accept-Encoding:"
	};

	void (HTTPRequest::*parsers[size])(const std::string&) = {
		&HTTPRequest::ParseHost,
		&HTTPRequest::ParseContentLength,
		&HTTPRequest::ParseUserAgent,
		&HTTPRequest::ParseAcceptEncoding
	};

	for (size_t i = 0; i < size; i++)
	{
		if (field == fields[i])
		{
			(this->*parsers[i])(content);
			return ;
		}
	}

	throw HTTPError(HTTPError::BAD_REQUEST);

	return ;
}

/*
void	HTTPRequest::ParseHeader(std::vector<std::string> const & list)
{
	const std::string	headers[2] = {
		"Host:",
		"Content-Length:",
		"User-Agent:",
		"Accept-Encoding:"
	};

	void (HTTPRequest::*parsers[2])(std::vector<std::string> const &) = {
		&HTTPRequest::ParseHost,
		&HTTPRequest::ParseContentLength,
		&HTTPRequest::ParseUserAgent,
		&HTTPRequest::ParseAcceptEncoding
	};

	for ( int i = 0; i < 2; i++)
	{
		if (list.at(0) == headers[i])
		{
			(this->*parsers[i])(list);
			return ;
		}
	}

	throw HTTPError(HTTPError::BAD_REQUEST);

	return ;
}
*/

void	HTTPRequest::ParseHeaders(ServerSocket const & ssocket)
{
	std::string				line;
	std::string				field;
	std::string				content;
	std::string::size_type	pos;

	while ((line = GetLine(ssocket)) != "")
	{
		pos = line.find(":");
		if (pos == std::string::npos)
			throw HTTPError(HTTPError::BAD_REQUEST);
		field = line.substr(0, pos);
		content = line.substr(pos + 1);
		//content = line.substr(pos + 1, line.size());
		ParseHeader(field, content);

		//std::vector<std::string>	list;
		//list = my_split(line, " ");
		//ParseHeader(list);
	}

	return ;
}

void	HTTPRequest::ParseBody(ServerSocket const & ssocket)
{
	std::string		data;
	std::string		tmp;
	size_t			remaining_byte;
	size_t			default_recv_byte = 1024;
	size_t			recv_byte;

	if (method_ != POST)
		return;

	remaining_byte = content_length_;

	if (save_.length() != 0)
	{
		tmp = save_;
		remaining_byte -= save_.length();
	}

	while (remaining_byte != 0)
	{
		if (default_recv_byte < remaining_byte)
			recv_byte = default_recv_byte;
		else
			recv_byte = remaining_byte;

		data = ssocket.RecvData(recv_byte);
		remaining_byte -= data.length();
		tmp += data;
	}

	body_ = tmp;

	return ;
}

void	HTTPRequest::ParseRequest(ServerSocket const & ssocket)
{
	ParseRequestLine(ssocket);
	ParseHeaders(ssocket);
	ParseBody(ssocket);

	return ;
}

void	HTTPRequest::RequestDisplay(void) const
{
	std::cout << "method            : " << method_ << std::endl;
	std::cout << "target            : " << target_ << std::endl;
	std::cout << "version           : " << version_ << std::endl;
	std::cout << "host              : " << host_ << std::endl;
	std::cout << "content_length    : " << content_length_ << std::endl;
	std::cout << "[ BODY ]" << std::endl;
	std::cout << body_ << std::endl;

	return ;
}
