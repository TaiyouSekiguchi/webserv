#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest(const ServerSocket& ssocket)
	: ssocket_(ssocket)
	, listen_(ssocket.GetListen())
	, server_confs_(ssocket.GetServerConfs())
	, server_conf_(server_confs_.at(0))
	, client_max_body_size_(0)
	, content_length_(0)
	, connection_(true)
{
}

HTTPRequest::~HTTPRequest()
{
}

const ServerDirective::Listen&		HTTPRequest::GetListen(void) const { return (listen_); }
const ServerDirective*				HTTPRequest::GetServerConf(void) const { return (server_conf_); }
std::string							HTTPRequest::GetMethod(void) const { return (method_); }
std::string							HTTPRequest::GetTarget(void) const { return (target_); }
std::string							HTTPRequest::GetVersion(void) const { return (version_); }
std::pair<std::string, std::string>	HTTPRequest::GetHost(void) const { return (host_); }
size_t								HTTPRequest::GetContentLength(void) const { return (content_length_); }
std::string							HTTPRequest::GetUserAgent(void) const { return (user_agent_); }
std::vector<std::string>			HTTPRequest::GetAcceptEncoding(void) const { return (accept_encoding_); }
bool								HTTPRequest::GetConnection(void) const { return (connection_); }
std::string							HTTPRequest::GetContentType(void) const { return (content_type_); };
std::string							HTTPRequest::GetBody(void) const { return (body_); }

bool	HTTPRequest::IsToken(const std::string& str)
{
	std::string::const_iterator		it;
	std::string::const_iterator		it_end;

	it = str.begin();
	it_end = str.end();

	for ( ; it != it_end; ++it)
	{
		if (!IsTChar(*it))
			return (false);
	}
	return (true);
}

bool	HTTPRequest::IsTChar(char c)
{
	if (c == '!'
		|| c == '#'
		|| c == '$'
		|| c == '%'
		|| c == '&'
		|| c == '\''
		|| c == '*'
		|| c == '+'
		|| c == '-'
		|| c == '.'
		|| c == '^'
		|| c == '_'
		|| c == '`'
		|| c == '|'
		|| c == '~'
		|| isdigit(c)
		|| isalpha(c))
	{
		return (true);
	}
	return (false);
}

std::string		HTTPRequest::GetLine(void)
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
		data = ssocket_.RecvData();
		if (data.size() == 0)
			throw ClientClosed();
		save_ += data;
	}

	line = save_.substr(0, pos);
	save_ = save_.substr(pos + separator_length, save_.size());

	return (line);
}

void	HTTPRequest::ParseMethod(const std::string& method)
{
	std::string::const_iterator	it;
	std::string::const_iterator	it_end;

	it = method.begin();
	it_end = method.end();
	for ( ; it != it_end; ++it)
	{
		if (!std::isupper(*it))
			throw HTTPError(BAD_REQUEST, "ParseMethod");
	}

	method_ = method;
}

void	HTTPRequest::ParseTarget(const std::string& target)
{
	if (target[0] != '/')
		throw HTTPError(BAD_REQUEST, "ParseTarget");

	target_ = target;
}

void	HTTPRequest::ParseVersion(const std::string& version)
{
	if (version == "HTTP/1.1")
		version_ = version;
	else
		throw HTTPError(HTTP_VERSION_NOT_SUPPORTED, "ParseVersion");
}

void	HTTPRequest::ParseRequestLine(void)
{
	std::string					line;
	std::vector<std::string>	list;

	while ((line = GetLine()) == "") { }

	if (Utils::IsBlank(line.at(0)))
		throw HTTPError(BAD_REQUEST, "ParseRequestLine");

	list = Utils::MySplit(line, " ");
	if (list.size() != 3)
		throw HTTPError(BAD_REQUEST, "ParseRequestLine");

	ParseMethod(list.at(0));
	ParseTarget(list.at(1));
	ParseVersion(list.at(2));

	return;
}

void HTTPRequest::ParseHost(const std::string& content)
{
	std::vector<std::string>	list;

	list = Utils::MySplit(content, ":");

	host_.first = Utils::MyTrim(list.at(0), " ");
	if (list.size() >= 2)
		host_.second = Utils::MyTrim(list.at(1), " ");

	FindServerConf();
}

void HTTPRequest::ParseContentLength(const std::string& content)
{
	std::string		tmp;
	char			*endptr;

	tmp = Utils::MyTrim(content, " ");
	content_length_ = std::strtoul(tmp.c_str(), &endptr, 10);
	if (errno == ERANGE || *endptr != '\0')
		throw HTTPError(BAD_REQUEST, "ParseContentLength");
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
		*it = Utils::MyTrim(*it, " ");
	accept_encoding_ = list;
}

void HTTPRequest::ParseConnection(const std::string& content)
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
		*it = Utils::StringToLower(*it);
		if (*it == "close")
		{
			connection_ = false;
			break;
		}
	}
}

void HTTPRequest::ParseContentType(const std::string& content)
{
	content_type_ = Utils::MyTrim(content, " ");
}

void	HTTPRequest::ParseHeader(const std::string& field, const std::string& content)
{
	const std::pair<std::string, ParseFunc> p[] = {
		std::make_pair("host", &HTTPRequest::ParseHost),
		std::make_pair("content-length", &HTTPRequest::ParseContentLength),
		std::make_pair("user-agent", &HTTPRequest::ParseUserAgent),
		std::make_pair("accept-encoding", &HTTPRequest::ParseAcceptEncoding),
		std::make_pair("connection", &HTTPRequest::ParseConnection),
		std::make_pair("content-type", &HTTPRequest::ParseContentType)
	};
	const std::map<std::string, ParseFunc>				parse_funcs(p, &p[6]);
	std::map<std::string, ParseFunc>::const_iterator	found;

	found = parse_funcs.find(field);
	if (found != parse_funcs.end())
		(this->*(found->second))(content);

	return;
}

void	HTTPRequest::ReceiveHeaders(void)
{
	std::string				array[6] = {
		"host",
		"content-length",
		"user-agent",
		"accept-encoding",
		"connection",
		"content-type"
	};
	std::vector<std::string>			header_list(array, array + 6);
	std::vector<std::string>::iterator	it;
	std::string							line;
	std::string							field;
	std::string							content;
	std::string::size_type				pos;

	while ((line = GetLine()) != "")
	{
		pos = line.find(":");
		if (pos == std::string::npos)
			continue;

		field = line.substr(0, pos);
		field = Utils::StringToLower(field);
		content = line.substr(pos + 1);

		it = std::find(header_list.begin(), header_list.end(), field);
		if (it != header_list.end())
		{
			if (headers_.count(field) == 0)
				headers_[field] = content;
			else
			{
				if (field == "host"
					|| field == "content-length")
					throw HTTPError(BAD_REQUEST, "ReceiveHeaders");
				else if (field == "accept-encoding"
					|| field == "connection")
					headers_[field] = headers_[field] + "," + content;
				else
					headers_[field] = content;
			}
		}
	}
}

void	HTTPRequest::ParseHeaders(void)
{
	std::map<std::string, std::string>::const_iterator	it;
	std::map<std::string, std::string>::const_iterator	it_end;

	it = headers_.begin();
	it_end = headers_.end();
	for ( ; it != it_end; ++it)
		ParseHeader(it->first, it->second);
}

void	HTTPRequest::CheckHeaders(void)
{
	if (host_.first == "")
		throw HTTPError(BAD_REQUEST, "CheckHeaders");

	client_max_body_size_ = server_conf_->GetClientMaxBodySize();
	if (client_max_body_size_ != 0 && content_length_ > client_max_body_size_)
		throw HTTPError(PAYLOAD_TOO_LARGE, "CheckHeaders");
}

void	HTTPRequest::ParseBody(void)
{
	std::string		data;
	std::string		tmp;
	size_t			remaining_byte;
	size_t			default_recv_byte = 1024;
	size_t			recv_byte;

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

		data = ssocket_.RecvData(recv_byte);
		remaining_byte -= data.length();
		tmp += data;
	}

	if (method_ == "POST")
		body_ = tmp;
}

void	HTTPRequest::FindServerConf(void)
{
	std::vector<const ServerDirective*>::const_iterator	sd_it;
	std::vector<const ServerDirective*>::const_iterator	sd_it_end;

	sd_it = server_confs_.begin();
	sd_it_end = server_confs_.end();
	server_conf_ = *sd_it;
	for ( ; sd_it != sd_it_end; ++sd_it)
	{
		const std::vector<std::string>&				server_names = (*sd_it)->GetServerNames();
		std::vector<std::string>::const_iterator	sn_it;
		std::vector<std::string>::const_iterator	sn_it_end;

		sn_it = server_names.begin();
		sn_it_end = server_names.end();
		for ( ; sn_it != sn_it_end; ++sn_it)
		{
			if (*sn_it == host_.first)
			{
				server_conf_ = *sd_it;
				return;
			}
		}
	}
}

void	HTTPRequest::ParseRequest(void)
{
	ParseRequestLine();
	ReceiveHeaders();
	ParseHeaders();
	CheckHeaders();
	ParseBody();

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
