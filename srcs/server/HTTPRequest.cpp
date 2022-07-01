#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest() : request_status_(PARSE), line_status_(REQUEST)
{
}

HTTPRequest::~HTTPRequest()
{
}

std::vector<std::string>	my_split(std::string const & str, std::string const & separator)
{
	std::vector<std::string>	list;
	std::string::size_type		separator_length;

	separator_length = separator.length();

	if (separator_length == 0)
		list.push_back(str);
	else
	{
		std::string::size_type	offset;

		offset = 0;
		while (1)
		{
			std::string::size_type pos = str.find(separator, offset);
			if (pos == std::string::npos)
			{
				list.push_back(str.substr(offset));
				break;
			}
			list.push_back(str.substr(offset, pos - offset));
			offset = pos + separator_length;
		}
	}

	return (list);
}

void	HTTPRequest::RequestPart(std::string const & line)
{
	std::vector<std::string>	list;

	list = my_split(line, " ");
	if (list.size() != 3)
	{
		request_status_ = BAD;
		return ;
	}
	
	std::string		tmp;

	tmp = list.at(0);
	if (tmp == "GET")
		method_ = GET;
	else
	{
		request_status_ = BAD;
		return ;
	}

	target_ = list.at(1);
	version_ = list.at(2);

	line_status_ = HEADER;

	return ;
}

void	HTTPRequest::HeaderPart(std::string const & line)
{
	std::vector<std::string>	list;

	list = my_split(line, " ");
	if (list.size() != 2)
	{
		request_status_ = BAD;
		return ;
	}

	std::string		field;

	field = list.at(0);
	if (field == "Host:")
		host_ = list.at(1);
	else
		request_status_ = BAD;

	return;
}

void	HTTPRequest::DoParse(std::string const & line)
{
	if (line == "")
		request_status_ = OK;
	else
	{
		if (line_status_ == REQUEST)
			RequestPart(line);
		else if (line_status_ == HEADER)
			HeaderPart(line);
		//else if (line_status_ == BODY)
			//BodyPart(recv_msg);
	}

	return ;
}

void	HTTPRequest::ParseRequest(std::string const & recv_msg)
{
	std::string				line;
	std::string				separator;
	std::string::size_type	separator_length;
	std::string::size_type	offset;

	separator = std::string("\r\n");
	separator_length = separator.length();

	offset = 0;
	while (1)
	{
		std::string::size_type	pos;

		pos = recv_msg.find(separator, offset);
		if (pos == std::string::npos)
			break;
		line = recv_msg.substr(offset, pos - offset);
		DoParse(line);
		if (request_status_ != PARSE)
			break;
		offset = pos + separator_length;
	}

	return ;
}

void	HTTPRequest::RequestDisplay(void) const
{
	std::cout << "Parse status [ " << request_status_ << " ]" << std::endl;
	std::cout << "method  : " << method_ << std::endl;
	std::cout << "target  : " << target_ << std::endl;
	std::cout << "version : " << version_ << std::endl;
	std::cout << "host    : " << host_ << std::endl;

	return ;
}

bool	HTTPRequest::GetParseStatus(void) const
{
	return (request_status_ != PARSE);
}
