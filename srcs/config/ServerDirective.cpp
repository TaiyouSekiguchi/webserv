#include "ServerDirective.hpp"

ServerDirective::ServerDirective(Tokens::citr begin, Tokens::citr end)
{
	const std::pair<std::string, SetFunc> p[] = {
		std::make_pair("listen", &ServerDirective::SetListen),
		std::make_pair("server_names", &ServerDirective::SetServerNames),
		std::make_pair("location", &ServerDirective::SetLocation)
	};
	const std::map<std::string, SetFunc>			set_funcs(p, &p[3]);
	std::map<std::string, SetFunc>::const_iterator	found;
	Tokens::citr									itr;
	Tokens::citr									directive_end;

	SetDefaultValues();

	itr = begin;
	while (itr != end)
	{
		found = set_funcs.find(*itr);
		if (found == set_funcs.end())
			throw std::runtime_error("conf syntax error");
		directive_end = GetDirectiveEnd(itr, end);
		if (directive_end == end)
			throw std::runtime_error("conf syntax error");
		(this->*(found->second))(itr + 1, directive_end);
		itr = directive_end + 1;
	}
}

ServerDirective::~ServerDirective()
{
}

const std::pair<std::string, int>&		ServerDirective::GetListen() const { return (listen_); }
const std::vector<std::string>&			ServerDirective::GetServerNames() const { return(server_names_); }
const std::vector<LocationDirective>&	ServerDirective::GetLocations() const { return (locations_); }

Tokens::citr	ServerDirective::GetDirectiveEnd(Tokens::citr begin, Tokens::citr end) const
{
	Tokens::citr	directive_end;

	if (*begin == "location")
		directive_end = Tokens::GetEndBracesItr(begin + 1, end);
	else
		directive_end = std::find(begin + 1, end, ";");
	return (directive_end);
}

void	ServerDirective::SetDefaultValues()
{
	// listen_ = std::make_pair("*", 80);
	// server_names_.push_back("");
	// client_max_body_size_ = 1048576;
}

void	ServerDirective::SetListen(Tokens::citr begin, Tokens::citr end)
{
	(void)begin;
	(void)end;
	listen_ = std::make_pair("*", std::atoi((*begin).c_str()));
}

void	ServerDirective::SetServerNames(Tokens::citr begin, Tokens::citr end)
{
	(void)begin;
	(void)end;
	server_names_.push_back(*begin);
}

void	ServerDirective::SetLocation(Tokens::citr begin, Tokens::citr end)
{
	if (begin == end || *begin != "{")
		throw std::runtime_error("conf syntax error");
	if (begin + 1 == end || Tokens::isSpecialToken(*(begin + 1)))
		throw std::runtime_error("conf syntax error");

	locations_.push_back(LocationDirective(*(begin + 1), begin + 2, end));
}
