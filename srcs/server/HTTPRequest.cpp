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

static std::vector<std::string>	my_split(std::string const & str, std::string const & separator)
{
	std::vector<std::string>	list;
	std::string::size_type		sep_len;
	std::string::size_type		offset;
	std::string::size_type		pos;

	sep_len = separator.length();

	if (sep_len == 0)
		list.push_back(str);
	else
	{
		offset = 0;
		while (1)
		{
			pos = str.find(separator, offset);
			if (pos == std::string::npos)
			{
				list.push_back(str.substr(offset));
				break;
			}
			list.push_back(str.substr(offset, pos - offset));
			offset = pos + sep_len;
			while (str.substr(offset, sep_len) == separator)
				offset += sep_len;
		}
	}

	return (list);
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
	if (target[0] != "/")
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

	list = my_split(line, " ");
	if (list.size() != 3)
		throw HTTPError(HTTPError::BAD_REQUEST);

	ParseMethod(list.at(0));
	ParseTarget(list.at(1));
	ParseVersion(list.at(2));

	return ;
}

void HTTPRequest::ParseHost(std::vector<std::string> const & list)
{
	if (list.size() != 2)
		throw HTTPError(HTTPError::BAD_REQUEST);

	host_ = list.at(1);
}

void HTTPRequest::ParseContentLength(std::vector<std::string> const & list)
{
	std::string		tmp;
	const char		*c_content_length;
	char			*endptr;

	if (list.size() != 2)
		throw HTTPError(HTTPError::BAD_REQUEST);

	tmp = list.at(1);
	c_content_length = tmp.c_str();
	content_length_ = std::strtoul(c_content_length, &endptr, 10);
	if (errno == ERANGE)
		throw HTTPError(HTTPError::BAD_REQUEST);
	if (*endptr != '\0')
		throw HTTPError(HTTPError::BAD_REQUEST);
}

void	HTTPRequest::ParseHeader(std::vector<std::string> const & list)
{
	const std::string	headers[2] = {
		"Host:",
		"Content-Length:",
	};

	void (HTTPRequest::*parsers[2])(std::vector<std::string> const &) = {
		&HTTPRequest::ParseHost,
		&HTTPRequest::ParseContentLength
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

void	HTTPRequest::ParseHeaders(ServerSocket const & ssocket)
{
	std::string		line;

	while ((line = GetLine(ssocket)) != "")
	{
		std::vector<std::string>	list;

		list = my_split(line, " ");
		ParseHeader(list);
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
