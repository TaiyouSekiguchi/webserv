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
	if (line_status_ == REQUEST)
	{
		if (line != "")
			RequestPart(line);
	}
	else if (line_status_ == HEADER)
	{
		if (line == "")
			line_status_ = BODY;
		else
			HeaderPart(line);
	}

	return ;
}

void	HTTPRequest::ParseRequest(ServerSocket & ssocket)
{
	std::string				data;
	std::string				save;
	std::string				separator;
	std::string::size_type	separator_length;
	std::string::size_type	pos;

	separator = "\r\n";
	separator_length = separator.length();

	while(line_status_ != BODY)
	{
		data = ssocket.RecvData();
		if (data.size() == 0)
		{
			//delete ssocket;
			break ;
		}
		save += data;
		while (line_status_ != BODY)
		{
			pos = save.find(separator);
			if (pos == std::string::npos)
				break;
			DoParse(save.substr(0, pos));
			save = save.substr(pos + separator_length, save.size());
		}
	}

	//Body part

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
