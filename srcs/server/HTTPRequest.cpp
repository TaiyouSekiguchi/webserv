#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest()
	: content_length_(0)
	, connection_(true)
{
	host_ = std::make_pair("", 80);
}

HTTPRequest::~HTTPRequest()
{
}

std::string						HTTPRequest::GetMethod(void) const { return (method_); }
std::string						HTTPRequest::GetTarget(void) const { return (target_); }
std::string						HTTPRequest::GetVersion(void) const { return (version_); }
std::pair<std::string, int>		HTTPRequest::GetHost(void) const { return (host_); }
size_t							HTTPRequest::GetContentLength(void) const { return (content_length_); }
std::string						HTTPRequest::GetUserAgent(void) const { return (user_agent_); }
std::vector<std::string>		HTTPRequest::GetAcceptEncoding(void) const { return (accept_encoding_); }
bool							HTTPRequest::GetConnection(void) const { return (connection_); }
std::string						HTTPRequest::GetContentType(void) const { return (content_type_); }
std::string						HTTPRequest::GetBody(void) const { return (body_); }


void	HTTPRequest::SetServerConf(const ServerDirective& server_conf)
{
	client_max_body_size_ = server_conf.GetClientMaxBodySize();
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
	const char *str = method.c_str();
	const char *found = std::find_if(str, str + method.size(), Utils::MyisLower);
	if (found != str + method.size())
		throw HTTPError(HTTPError::BAD_REQUEST);
	method_ = method;
}

void	HTTPRequest::ParseTarget(const std::string& target)
{
	if (target[0] != '/')
		throw HTTPError(HTTPError::BAD_REQUEST);

	target_ = target;
}

void	HTTPRequest::ParseVersion(const std::string& version)
{
	if (version == "HTTP/1.1")
		version_ = version;
	else
		throw HTTPError(HTTPError::HTTP_VERSION_NOT_SUPPORTED);
}

void	HTTPRequest::ParseRequestLine(const ServerSocket& ssocket)
{
	std::string					line;
	std::vector<std::string>	list;

	while ((line = GetLine(ssocket)) == "") { }

	list = Utils::MySplit(line, " ");
	if (list.size() != 3)
		throw HTTPError(HTTPError::BAD_REQUEST);

	ParseMethod(list.at(0));
	ParseTarget(list.at(1));
	ParseVersion(list.at(2));

	return;
}

void HTTPRequest::ParseHost(const std::string& content)
{
	std::vector<std::string>	list1;
	std::vector<std::string>	list2;
	std::string					host;

	list1 = Utils::MySplit(content, " ");
	if (list1.size() != 1)
		throw HTTPError(HTTPError::BAD_REQUEST);

	list2 = Utils::MySplit(list1.at(0), ":");
	if (list2.size() > 2)
		throw HTTPError(HTTPError::BAD_REQUEST);

	host = list2.at(0);
	for (size_t i = 0; i < host.size(); i++)
	{
		if (!isalpha(host[i])
			&& !isdigit(host[i])
			&& host[i] != '.'
			&& host[i] != '-')
			throw HTTPError(HTTPError::BAD_REQUEST);
	}
	host_.first = host;

	if (list2.size() == 2)
	{
		long	port;
		char	*endptr;

		port = std::strtol(list2.at(1).c_str(), &endptr, 10);
		if (*endptr != '\0' || errno == ERANGE || port < 1 || 65535 < port)
			throw HTTPError(HTTPError::BAD_REQUEST);
		host_.second = port;
	}
}

void HTTPRequest::ParseContentLength(const std::string& content)
{
	std::vector<std::string>	list;
	char						*endptr;

	list = Utils::MySplit(content, " ");
	if (list.size() != 1)
		throw HTTPError(HTTPError::BAD_REQUEST);

	content_length_ = std::strtoul(list.at(0).c_str(), &endptr, 10);
	if (errno == ERANGE || *endptr != '\0')
		throw HTTPError(HTTPError::BAD_REQUEST);
	if (client_max_body_size_ != 0 && content_length_ > client_max_body_size_)
		throw HTTPError(HTTPError::PAYLOAD_TOO_LARGE);
}

void HTTPRequest::ParseUserAgent(const std::string& content)
{
	user_agent_ = Utils::MyTrim(content, " ");
}

void HTTPRequest::ParseAcceptEncoding(const std::string& content)
{
	std::vector<std::string>			list;
	std::vector<std::string>::iterator	it;
	std::vector<std::string>::iterator	it_end;

	list = Utils::MySplit(content, ",");
	it = list.begin();
	it_end = list.end();
	for (; it != it_end; ++it)
	{
		*it = Utils::MyTrim(*it, " ");
	}
	accept_encoding_ = list;
}

void HTTPRequest::ParseConnection(const std::string& content)
{
	std::string		tmp;

	tmp = Utils::MyTrim(content, " ");
	for (size_t i = 0; i < content.length(); i++)
	{
		if (tmp[i] >= 'A' && tmp[i] <= 'Z')
			tmp[i] = tolower(tmp[i]);
	}

	if (tmp == "close")
		connection_ = false;
}

void HTTPRequest::ParseContentType(const std::string& content)
{
	content_type_ = Utils::MyTrim(content, " ");
}

void	HTTPRequest::ParseHeader(const std::string& field, const std::string& content)
{
	const std::pair<std::string, ParseFunc> p[] = {
		std::make_pair("Host", &HTTPRequest::ParseHost),
		std::make_pair("Content-Length", &HTTPRequest::ParseContentLength),
		std::make_pair("User-Agent", &HTTPRequest::ParseUserAgent),
		std::make_pair("Accept-Encoding", &HTTPRequest::ParseAcceptEncoding),
		std::make_pair("Connection", &HTTPRequest::ParseConnection),
		std::make_pair("Content-Type", &HTTPRequest::ParseContentType)
	};
	const std::map<std::string, ParseFunc>				parse_funcs(p, &p[6]);
	std::map<std::string, ParseFunc>::const_iterator	found;

	found = parse_funcs.find(field);
	if (found != parse_funcs.end())
		(this->*(found->second))(content);

	return;
}

void	HTTPRequest::ParseHeaders(const ServerSocket& ssocket)
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
		ParseHeader(field, content);
	}

	if (host_.first == "")
		throw HTTPError(HTTPError::BAD_REQUEST);

	return;
}

void	HTTPRequest::ParseBody(ServerSocket const & ssocket)
{
	std::string		data;
	std::string		tmp;
	size_t			remaining_byte;
	size_t			default_recv_byte = 1024;
	size_t			recv_byte;

	// if (method_ != POST)
	// 	return;

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

	return;
}

void	HTTPRequest::ParseRequest(const ServerSocket& ssocket, const ServerDirective& server_conf)
{
	SetServerConf(server_conf);
	ParseRequestLine(ssocket);
	ParseHeaders(ssocket);
	ParseBody(ssocket);

	return;
}

void	HTTPRequest::RequestDisplay(void) const
{
	std::cout << "method            : " << method_ << std::endl;
	std::cout << "target            : " << target_ << std::endl;
	std::cout << "version           : " << version_ << std::endl;
	std::cout << "host.first        : " << host_.first << std::endl;
	std::cout << "host.second       : " << host_.second << std::endl;
	std::cout << "content_length    : " << content_length_ << std::endl;
	std::cout << "[ BODY ]" << std::endl;
	std::cout << body_ << std::endl;

	return;
}
