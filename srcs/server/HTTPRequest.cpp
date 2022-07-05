#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest()
	: method_(NONE)
	, content_length_(0)
{
}

HTTPRequest::~HTTPRequest()
{
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

void	HTTPRequest::ParseRequestLine(ServerSocket const & ssocket)
{
	std::string					line;
	std::vector<std::string>	list;
	const std::string			methods[3] = { "GET", "POST", "DELETE" };

	while ((line = GetLine(ssocket)) == "")
		;

	list = my_split(line, " ");
	if (list.size() != 3)
		throw HTTPError(HTTPError::BAD_REQUEST);

	for (int i = 0; i < 3; i++)
	{
		if (list.at(0) == methods[i])
			method_ = static_cast<HTTPRequest::e_method>(i);
	}
	if (method_ == NONE)
		throw HTTPError(HTTPError::BAD_REQUEST);

	target_ = list.at(1);

	if (list.at(2) == "HTTP/1.0" || list.at(2) == "HTTP/1.1")
		version_ = list.at(2);
	else
		throw HTTPError(HTTPError::BAD_REQUEST);

	return ;
}

void	HTTPRequest::ParseHeaders(ServerSocket const & ssocket)
{
	std::string		line;

	while ((line = GetLine(ssocket)) != "")
	{
		std::vector<std::string>	list;
		std::string					field;

		list = my_split(line, " ");
		if (list.size() != 2)
			throw std::exception();

		field = list.at(0);
		if (field == "Host:")
			host_ = list.at(1);
		else if (field == "Content-Length:")
		{
			std::string		tmp;
			const char		*c_content_length;
			char			*endptr;

			tmp = list.at(1);
			c_content_length = tmp.c_str();
			content_length_ = std::strtoul(c_content_length, &endptr, 10);
			if (errno == ERANGE)
				throw HTTPError(HTTPError::BAD_REQUEST);
			if (*endptr != '\0')
				throw HTTPError(HTTPError::BAD_REQUEST);
		}
		else
			throw std::exception();
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
