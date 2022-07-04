#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest()
	: method_(NONE)
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

	line = GetLine(ssocket);

	list = my_split(line, " ");
	if (list.size() != 3)
		throw std::exception();

	for (int i = 0; i < 3; i++)
	{
		if (list.at(0) == methods[i])
			method_ = static_cast<HTTPRequest::e_method>(i);
	}
	if (method_ == NONE)
		throw std::exception();

	target_ = list.at(1);

	if (list.at(2) == "HTTP/1.0" || list.at(2) == "HTTP/1.1")
		version_ = list.at(2);
	else
		throw std::exception();

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
		else
			throw std::exception();
	}

	return ;
}

void	HTTPRequest::ParseRequest(ServerSocket const & ssocket)
{
	ParseRequestLine(ssocket);
	ParseHeaders(ssocket);
	//ParseBody();

	return ;
}

void	HTTPRequest::RequestDisplay(void) const
{
	std::cout << "method  : " << method_ << std::endl;
	std::cout << "target  : " << target_ << std::endl;
	std::cout << "version : " << version_ << std::endl;
	std::cout << "host    : " << host_ << std::endl;

	return ;
}
