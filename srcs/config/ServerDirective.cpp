#include <arpa/inet.h>
#include "ServerDirective.hpp"

ServerDirective::ServerDirective(Tokens::citr begin, Tokens::citr end)
{
	const std::pair<std::string, ParseFunc> p[] = {
		std::make_pair("listen", &ServerDirective::ParseListen),
		std::make_pair("server_name", &ServerDirective::ParseServerNames),
		std::make_pair("location", &ServerDirective::ParseLocation)
	};
	const std::map<std::string, ParseFunc>				parse_funcs(p, &p[3]);
	std::map<std::string, ParseFunc>::const_iterator	found;
	Tokens::citr										itr;
	Tokens::citr										directive_end;

	SetDefaultValues();

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
}

ServerDirective::~ServerDirective()
{
}

const std::pair<unsigned int, int>&		ServerDirective::GetListen() const { return (listen_); }
const std::vector<std::string>&			ServerDirective::GetServerNames() const { return(server_names_); }
const std::vector<LocationDirective>&	ServerDirective::GetLocations() const { return (locations_); }

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

void	ServerDirective::SetDefaultValues()
{
	listen_ = std::make_pair(INADDR_ANY, 80);
	server_names_.push_back("");
	// client_max_body_size_ = 1048576;
}

void	ServerDirective::ParseListen(Tokens::citr begin, Tokens::citr end)
{
	if (begin + 1 != end)
		throw std::runtime_error("conf syntax error");

	in_addr_t						ip;
	int								port;
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
		listen_.first = ip;
	}
	if (colon != std::string::npos || period == std::string::npos)
	{
		if (colon != std::string::npos)
			port = std::strtol(s.substr(colon + 1).c_str(), &endptr, 10);
		else
			port = std::strtol(s.c_str(), &endptr, 10);
		if (*endptr != '\0' || errno == ERANGE || port < 1 || 65535 < port)
			throw std::runtime_error("conf syntax error");
		listen_.second = port;
	}
}

void	ServerDirective::ParseServerNames(Tokens::citr begin, Tokens::citr end)
{
	server_names_.clear();

	Tokens::citr	itr = begin;
	while (itr != end)
	{
		if (Tokens::isSpecialToken(*itr))
			throw std::runtime_error("conf syntax error");
		server_names_.push_back(*itr);
		itr++;
	}
}

void	ServerDirective::ParseLocation(Tokens::citr begin, Tokens::citr end)
{
	if (begin >= end || Tokens::isSpecialToken(*begin))
		throw std::runtime_error("conf syntax error");

	locations_.push_back(LocationDirective(*begin, begin + 2, end));
}
