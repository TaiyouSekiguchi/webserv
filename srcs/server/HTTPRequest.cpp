#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest(const ServerSocket& ssocket)
	: ssocket_(ssocket)
	, listen_(ssocket.GetListen())
	, server_confs_(ssocket.GetServerConfs())
	, server_conf_(server_confs_.at(0))
	, client_max_body_size_(0)
	, step_(0)
	, content_length_(0)
	, connection_(true)
	, parse_pos_(0)
{
}

HTTPRequest::~HTTPRequest()
{
}

const ServerDirective::Listen&	HTTPRequest::GetListen(void) const { return (listen_); }
const ServerDirective*			HTTPRequest::GetServerConf(void) const { return (server_conf_); }
std::string						HTTPRequest::GetMethod(void) const { return (method_); }
std::string						HTTPRequest::GetTarget(void) const { return (target_); }
std::string						HTTPRequest::GetVersion(void) const { return (version_); }
std::pair<std::string, std::string>	HTTPRequest::GetHost(void) const { return (host_); }
size_t							HTTPRequest::GetContentLength(void) const { return (content_length_); }
std::string						HTTPRequest::GetUserAgent(void) const { return (user_agent_); }
std::vector<std::string>		HTTPRequest::GetAcceptEncoding(void) const { return (accept_encoding_); }
bool							HTTPRequest::GetConnection(void) const { return (connection_); }
std::string						HTTPRequest::GetContentType(void) const { return (content_type_); };
std::string						HTTPRequest::GetBody(void) const { return (body_); }
std::string						HTTPRequest::GetAccept(void) const { return (accept_); }
std::pair<std::string, std::string>	HTTPRequest::GetAuthorization(void) const { return (authorization_); }


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

bool		HTTPRequest::GetLine(std::string* line)
{
	std::string				separator;
	std::string::size_type	separator_length;
	std::string::size_type	pos;

	separator = "\r\n";
	separator_length = separator.length();

	pos = save_.find(separator);
	if (pos == std::string::npos)
		return (false);
	*line = save_.substr(0, pos);
	save_ = save_.substr(pos + separator_length, save_.size());
	return (true);
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
			throw HTTPError(SC_BAD_REQUEST, "ParseMethod");
	}

	method_ = method;
}

void	HTTPRequest::ParseTarget(const std::string& target)
{
	if (target[0] != '/')
		throw HTTPError(SC_BAD_REQUEST, "ParseTarget");

	target_ = target;
}

void	HTTPRequest::ParseVersion(const std::string& version)
{
	const char*	tmp;
	size_t		i;

	if (version.compare(0, 5, "HTTP/"))
		throw HTTPError(SC_BAD_REQUEST, "ParseVersion");

	tmp = version.c_str();

	i = 5;
	while (isdigit(tmp[i]))
		i++;

	if (i == 5 || tmp[i++] != '.' || tmp[i] == '\0')
		throw HTTPError(SC_BAD_REQUEST, "ParseVersion");

	while (isdigit(tmp[i]))
		i++;

	if (tmp[i] != '\0')
		throw HTTPError(SC_BAD_REQUEST, "ParseVersion");

	if (version != "HTTP/1.1")
		throw HTTPError(SC_HTTP_VERSION_NOT_SUPPORTED, "ParseVersion");

	version_ = version;
}

bool	HTTPRequest::ReceiveRequestLine(void)
{
	std::string					line;
	std::vector<std::string>	list;

	while (line == "")
	{
		if (!GetLine(&line))
			return (false);
	}

	if (Utils::IsBlank(line.at(0)))
		throw HTTPError(SC_BAD_REQUEST, "ParseRequestLine");

	list = Utils::MySplit(line, " ");
	if (list.size() != 3)
		throw HTTPError(SC_BAD_REQUEST, "ParseRequestLine");

	ParseMethod(list.at(0));
	ParseTarget(list.at(1));
	ParseVersion(list.at(2));

	return (true);
}

void HTTPRequest::ParseHost(const std::string& content)
{
	std::vector<std::string>	list;

	if (host_.first != "")
		throw HTTPError(SC_BAD_REQUEST, "ReceiveHeaders");

	list = Utils::MySplit(Utils::MyTrim(content), ":");
	if (list.empty())
		throw HTTPError(SC_BAD_REQUEST, "ParseHost");

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
	if (tmp.empty())
		throw HTTPError(SC_BAD_REQUEST, "ParseContentLength");
	content_length_ = std::strtoul(tmp.c_str(), &endptr, 10);
	if (errno == ERANGE || *endptr != '\0')
		throw HTTPError(SC_BAD_REQUEST, "ParseContentLength");
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
	std::string							encoding;

	list = Utils::MySplit(content, ",");
	it = list.begin();
	it_end = list.end();
	for (; it != it_end; ++it)
	{
		encoding = Utils::StringToLower(Utils::MyTrim(*it, " "));
		if (!encoding.empty() && Utils::IsNotFound(accept_encoding_, encoding))
			accept_encoding_.push_back(encoding);
	}
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

void HTTPRequest::ParseTransferEncoding(const std::string& content)
{
	std::vector<std::string>			list;
	std::vector<std::string>::iterator	it;
	std::vector<std::string>::iterator	it_end;

	transfer_encoding_ = Utils::MyTrim(content);
	transfer_encoding_ = Utils::StringToLower(transfer_encoding_);
	if (transfer_encoding_ != "chunked")
		throw HTTPError(SC_NOT_IMPLEMENTED, "ParseTransferEncoding");
}

void HTTPRequest::ParseAccept(const std::string& content)
{
	std::set<std::string>				accept_set;
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
		accept_set.insert(*it);
	}

	std::set<std::string>::iterator	sit;
	std::set<std::string>::iterator	sit_end;
	sit = accept_set.begin();
	sit_end = accept_set.end();
	for (; sit != sit_end; ++sit)
	{
		if (sit != accept_set.begin())
			accept_ += ",";
		accept_ += *sit;
	}
}

void HTTPRequest::ParseAuthorization(const std::string& content)
{
	std::string tmp;

	tmp = Utils::MyTrim(content, " ");
	if (tmp.empty())
		return;
	std::string::size_type end = tmp.find(" ");
	if (end == std::string::npos)
		authorization_.first = tmp;
	else
	{
		authorization_.first = tmp.substr(0, end);
		authorization_.second = tmp.substr(end + 1);
	}
}

void	HTTPRequest::ParseHeader(const std::string& field, const std::string& content)
{
	const std::pair<std::string, ParseFunc> p[] = {
		std::make_pair("content-length", &HTTPRequest::ParseContentLength),
		std::make_pair("user-agent", &HTTPRequest::ParseUserAgent),
		std::make_pair("accept-encoding", &HTTPRequest::ParseAcceptEncoding),
		std::make_pair("connection", &HTTPRequest::ParseConnection),
		std::make_pair("content-type", &HTTPRequest::ParseContentType),
		std::make_pair("transfer-encoding", &HTTPRequest::ParseTransferEncoding),
		std::make_pair("accept", &HTTPRequest::ParseAccept),
		std::make_pair("authorization", &HTTPRequest::ParseAuthorization)
	};
	const std::map<std::string, ParseFunc>				parse_funcs(p, &p[8]);
	std::map<std::string, ParseFunc>::const_iterator	found;

	found = parse_funcs.find(field);
	if (found != parse_funcs.end())
		(this->*(found->second))(content);
	return;
}

static bool	IsOnlyOnceHeader(const std::string& field)
{
	if (field == "content-length"
		|| field == "transfer-encoding"
		|| field == "authorization")
		return (true);
	return (false);
}

static bool	IsAppendHeader(const std::string& field)
{
	if (field == "accept-encoding"
		|| field == "connection"
		|| field == "accept")
		return (true);
	return (false);
}

void	HTTPRequest::RegisterHeaders(const std::string& field, const std::string& content)
{
	if (field == "host")
		return (ParseHost(content));

	if (headers_.count(field) == 0)
		headers_[field] = content;
	else
	{
		if (IsOnlyOnceHeader(field))
			throw HTTPError(SC_BAD_REQUEST, "ReceiveHeaders");
		else if (IsAppendHeader(field))
			headers_[field] = headers_[field] + "," + content;
		else
			headers_[field] = content;
	}
}

bool	HTTPRequest::ReceiveHeaders(void)
{
	std::string		array[9] = {
		"host",
		"content-length",
		"user-agent",
		"accept-encoding",
		"connection",
		"content-type",
		"transfer-encoding",
		"accept",
		"authorization"
	};
	std::vector<std::string>	headers(array, array + 9);
	std::string					line;
	std::string					field;
	std::string					content;
	std::string::size_type		pos;

	while (1)
	{
		if (!GetLine(&line))
			return (false);
		if (line == "")
			break;
		if ((pos = line.find(":")) == std::string::npos)
			continue;

		field = line.substr(0, pos);
		content = line.substr(pos + 1);
		field = Utils::StringToLower(field);

		if (std::find(headers.begin(), headers.end(), field) != headers.end())
			RegisterHeaders(field, content);
	}
	ParseHeaders();
	CheckHeaders();
	return (true);
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
		throw HTTPError(SC_BAD_REQUEST, "CheckHeaders");

	if (headers_.count("content-length") && transfer_encoding_ == "chunked")
		throw HTTPError(SC_BAD_REQUEST, "CheckHeaders");

	client_max_body_size_ = server_conf_->GetClientMaxBodySize();
	if (client_max_body_size_ != 0 && content_length_ > client_max_body_size_)
		throw HTTPError(SC_PAYLOAD_TOO_LARGE, "CheckHeaders");
}

void	HTTPRequest::ParseChunkSize(void)
{
	const std::string	LINE_END = "\r\n";
	size_t		line_end_pos;
	std::string	line;

	line_end_pos = raw_body_.find(LINE_END, parse_pos_);
	if (line_end_pos == std::string::npos)
		throw HTTPError(SC_BAD_REQUEST, "ParseChunkSize");

	line = raw_body_.substr(parse_pos_, line_end_pos - parse_pos_);
	size_t c_pos = line.find(";");
	if (c_pos != std::string::npos)
		line = line.substr(0, c_pos);

	line = Utils::MyTrim(line, " ");
	for (size_t i = 0; i < line.size(); i++)
		if (!isxdigit(line.at(i)))
			throw HTTPError(SC_BAD_REQUEST, "ParseChunkSize");

	char *endptr;
	chunk_size_ = strtol(line.c_str(), &endptr, 16);
	if (errno == ERANGE || *endptr != '\0')
		throw HTTPError(SC_BAD_REQUEST, "ParseChunkSize");

	parse_pos_ = line_end_pos + LINE_END.size();
}

void	HTTPRequest::ParseChunkData(void)
{
	const std::string	LINE_END = "\r\n";
	std::string	line;

	if (raw_body_.size() - parse_pos_ < chunk_size_ + LINE_END.size())
		throw HTTPError(SC_BAD_REQUEST, "ParseChunkData");
	line = raw_body_.substr(parse_pos_, chunk_size_);
	body_ += line;
	parse_pos_ += chunk_size_ + LINE_END.size();
}

bool	HTTPRequest::ParseChunk(void)
{
	const std::string	FOOTER = "0\r\n\r\n";
	const std::string	LINE_END = "\r\n";
	size_t	remaining_byte;

	if (raw_body_.size() < 5 || raw_body_.compare(raw_body_.size() - 5, 5, FOOTER))
		return (false);
	if (raw_body_.size() > client_max_body_size_)
		throw HTTPError(SC_PAYLOAD_TOO_LARGE, "ParseChunk");

	while (1)
	{
		remaining_byte = raw_body_.size() - parse_pos_;
		if (remaining_byte <= 5)
		{
			if (remaining_byte == 5)
				break;
			else
				throw HTTPError(SC_BAD_REQUEST, "ParseChunk");
		}
		ParseChunkSize();
		ParseChunkData();
	}
	return (true);
}

bool	HTTPRequest::ReceiveBody(void)
{
	raw_body_ += save_;
	save_ = "";
	if (transfer_encoding_ == "chunked")
	{
		if (!ParseChunk())
			return (false);
		else
		{
			content_length_ = body_.size();
			return (true);
		}
	}
	body_ = raw_body_;

	if (body_.size() < content_length_)
		return (false);
	else if (body_.size() == content_length_)
		return (true);
	else
		throw HTTPError(SC_BAD_REQUEST, "ReceiveBody");
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

e_HTTPServerEventType	HTTPRequest::ParseRequest(void)
{
	std::string	recv_data;
	ssize_t 	recv_size = ssocket_.RecvData(&recv_data);
	if (recv_size <= 0)
		return (SEVENT_END);
	save_ += recv_data;

	if (step_ == 0 && ReceiveRequestLine())
		step_++;
	if (step_ == 1 && ReceiveHeaders())
		step_++;
	if (step_ == 2 && ReceiveBody())
		step_++;

	if (step_ == 3)
		return (SEVENT_NO);
	else
		return (SEVENT_SOCKET_RECV);
}

void	HTTPRequest::RequestDisplay(void) const
{
	std::cout << "method            : " << method_ << std::endl;
	std::cout << "target            : " << target_ << std::endl;
	std::cout << "version           : " << version_ << std::endl;
	std::cout << "host.first        : " << host_.first << std::endl;
	std::cout << "host.second       : " << host_.second << std::endl;
	std::cout << "content_length    : " << content_length_ << std::endl;
	std::cout << "transfer-encoding : " << transfer_encoding_ << std::endl;
	std::cout << "[ BODY ]" << std::endl;
	std::cout << body_ << std::endl;

	return;
}

void	HTTPRequest::HeadersDisplay(void)
{
	std::map<std::string, std::string>::const_iterator	it;
	std::map<std::string, std::string>::const_iterator	it_end;

	it = headers_.begin();
	it_end = headers_.end();
	for (; it != it_end; ++it)
		std::cout << it->first << ": " << it->second << std::endl;
}
