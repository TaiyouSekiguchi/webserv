#include <arpa/inet.h>
#include "ServerDirective.hpp"
#include "utils.hpp"

ServerDirective::ServerDirective(Tokens::citr begin, Tokens::citr end)
{
	const std::pair<std::string, ParseFunc> p[] = {
		std::make_pair("listen", &ServerDirective::ParseListen),
		std::make_pair("server_name", &ServerDirective::ParseServerNames),
		std::make_pair("error_page", &ServerDirective::ParseErrorPages),
		std::make_pair("client_max_body_size", &ServerDirective::ParseClientMaxBodySize),
		std::make_pair("location", &ServerDirective::ParseLocation)
	};
	const std::map<std::string, ParseFunc>				parse_funcs(p, &p[5]);
	std::map<std::string, ParseFunc>::const_iterator	found;
	Tokens::citr										itr;
	Tokens::citr										directive_end;

	SetInitValue();
	itr = begin;
	while (itr < end)
	{
		found = parse_funcs.find(*itr);
		if (found == parse_funcs.end())
			throw std::runtime_error("conf syntax error");
		directive_end = GetDirectiveEnd(*itr, itr + 1, end);
		if (directive_end == end || itr + 1 == directive_end)
			throw std::runtime_error("conf syntax error");
		(this->*(found->second))(itr + 1, directive_end);
		itr = directive_end + 1;
	}
	SetDefaultValue();
}

ServerDirective::~ServerDirective()
{
}

const std::vector<std::pair<unsigned int, int> >&	ServerDirective::GetListen() const { return (listen_); }
const std::vector<std::string>&						ServerDirective::GetServerNames() const { return(server_names_); }
const std::map<int, std::string>&					ServerDirective::GetErrorPages() const { return (error_pages_); }
const long&											ServerDirective::GetClientMaxBodySize() const { return (client_max_body_size_); }
const std::vector<LocationDirective>&				ServerDirective::GetLocations() const { return (locations_); }

Tokens::citr	ServerDirective::GetDirectiveEnd
	(const std::string& name, Tokens::citr begin, Tokens::citr end) const
{
	Tokens::citr	directive_end;

	if (name == "location")
		directive_end = Tokens::GetEndBracesItr(begin + 1, end);
	else
		directive_end = std::find(begin, end, ";");
	return (directive_end);
}

void	ServerDirective::SetInitValue()
{
	errno = 0;
	client_max_body_size_ = -1;
}

void	ServerDirective::SetDefaultValue()
{
	if (listen_.size() == 0)
		listen_.push_back(std::make_pair(INADDR_ANY, 8000));
	if (server_names_.size() == 0)
		server_names_.push_back("");
	if (client_max_body_size_ == -1)
		client_max_body_size_ = 1048576;
	if (locations_.size() == 0)
		locations_.push_back(LocationDirective());
}

void	ServerDirective::ParseListen(Tokens::citr begin, Tokens::citr end)
{
	if (begin + 1 != end)
		throw std::runtime_error("conf syntax error");

	in_addr_t						ip = INADDR_ANY;
	long							port = 80;
	std::string						port_str;
	char							*endptr;
	const std::string&				s = *begin;
	const std::string::size_type	colon = s.find(':');
	const std::string::size_type	period = s.find('.');

	if (colon != std::string::npos || period != std::string::npos)
	{
		if (colon != std::string::npos)
			ip = inet_addr(s.substr(0, colon).c_str());
		else
			ip = inet_addr(s.c_str());
		if (ip == INADDR_NONE)
			throw std::runtime_error("conf syntax error");
	}
	if (colon != std::string::npos || period == std::string::npos)
	{
		if (colon != std::string::npos)
			port_str = s.substr(colon + 1);
		else
			port_str = s;
		port = std::strtol(port_str.c_str(), &endptr, 10);
		if (*endptr != '\0' || errno == ERANGE || port < 1 || 65535 < port)
			throw std::runtime_error("conf syntax error");
	}
	std::pair<unsigned int, int>	new_listen = std::make_pair(ip, port);
	if (!Utils::IsNotFound(listen_, new_listen))
		throw std::runtime_error("conf syntax error");
	listen_.push_back(new_listen);
}

void	ServerDirective::ParseServerNames(Tokens::citr begin, Tokens::citr end)
{
	Tokens::citr	itr = begin;
	while (itr != end)
	{
		if (Tokens::isSpecialToken(*itr))
			throw std::runtime_error("conf syntax error");
		if (Utils::IsNotFound(server_names_, *itr))
			server_names_.push_back(*itr);
		itr++;
	}
}

void	ServerDirective::ParseErrorPages(Tokens::citr begin, Tokens::citr end)
{
	if (begin + 2 > end)
		throw std::runtime_error("conf syntax error");

	const std::string		error_file = *(end - 1);
	long					status_code;
	char					*endptr;
	Tokens::citr			itr = begin;

	while (itr != end - 1)
	{
		status_code = std::strtol((*itr).c_str(), &endptr, 10);
		if (*endptr != '\0' || errno == ERANGE || status_code < 300 || 599 < status_code)
			throw std::runtime_error("conf syntax error");
		if (error_pages_.find(status_code) == error_pages_.end())
			error_pages_[status_code] = error_file;
		++itr;
	}
}

void	ServerDirective::ParseClientMaxBodySize(Tokens::citr begin, Tokens::citr end)
{
	if (begin + 1 != end)
		throw std::runtime_error("conf syntax error");
	else if (client_max_body_size_ != -1)
		throw std::runtime_error("conf syntax error");

	char		*endptr;
	client_max_body_size_ = std::strtol((*begin).c_str(), &endptr, 10);
	if (errno == ERANGE)
		throw std::runtime_error("conf syntax error");
	if (*endptr == '\0')
		return;

	const char	unit_chars[6] = {'k', 'K', 'm', 'M', 'g', 'G'};
	const char *unit_found = std::find(unit_chars, &unit_chars[6], *endptr);
	if (unit_found == &unit_chars[6] || *(endptr + 1) != '\0')
		throw std::runtime_error("conf syntax error");

	const int	unit_nums[6] = {1024, 1024, 1048576, 1048576, 1073741824, 1073741824};
	const int	unit_num = unit_nums[unit_found - unit_chars];
	const long	old = client_max_body_size_;
	client_max_body_size_ *= unit_num;
	if (client_max_body_size_ / unit_num != old)
		throw std::runtime_error("conf syntax error");
}

void	ServerDirective::ParseLocation(Tokens::citr begin, Tokens::citr end)
{
	if (begin >= end || Tokens::isSpecialToken(*begin))
		throw std::runtime_error("conf syntax error");

	const std::string& 								path = *begin;
	std::vector<LocationDirective>::const_iterator	found;

	found = Utils::FindMatchMember(locations_, &LocationDirective::GetPath, path);
	if (found != locations_.end())
		throw std::runtime_error("conf syntax error");
	locations_.push_back(LocationDirective(path, begin + 2, end));
}
