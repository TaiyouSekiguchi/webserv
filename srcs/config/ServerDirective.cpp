#include "ServerDirective.hpp"

ServerDirective::ServerDirective(Tokens::citr begin, Tokens::citr server_end)
{
	const std::pair<std::string, SetFunc> p[] = {
		std::make_pair("listen", &ServerDirective::SetListen),
		std::make_pair("server_names", &ServerDirective::SetServerNames),
		std::make_pair("location", &ServerDirective::SetLocation)
	};
	const std::map<std::string, SetFunc>			set_funcs(p, &p[3]);
	std::map<std::string, SetFunc>::const_iterator	found;
	Tokens::citr									itr;
	int												advanced_len;

	InitDirectives();

	itr = begin;
	while (itr != server_end)
	{
		found = set_funcs.find(*itr);
		if (found == set_funcs.end())
			throw std::runtime_error("conf syntax error");
		(this->*(found->second))(++itr, server_end, &advanced_len);
		itr += advanced_len;
	}
}

ServerDirective::~ServerDirective()
{
}

const std::pair<std::string, int>&		ServerDirective::GetListen() const { return (listen_); }
const std::vector<std::string>&			ServerDirective::GetServerNames() const { return(server_names_); }
const std::vector<LocationDirective>&	ServerDirective::GetLocations() const { return (locations_); }

void	ServerDirective::InitDirectives()
{
	listen_ = std::make_pair("*", 80);
	server_names_.push_back("");
	// client_max_body_size_ = 1048576;
}

void	ServerDirective::SetListen(Tokens::citr begin, Tokens::citr server_end, int *advanced_len)
{
	(void)begin;
	(void)server_end;
	listen_ = std::make_pair("*", 8080);
	*advanced_len = 3;
}

void	ServerDirective::SetServerNames(Tokens::citr begin, Tokens::citr server_end, int *advanced_len)
{
	(void)begin;
	(void)server_end;
	server_names_.push_back("localhost");
	*advanced_len = 3;
}

void	ServerDirective::SetLocation(Tokens::citr begin, Tokens::citr server_end, int *advanced_len)
{
	(void)begin;
	(void)server_end;
	locations_.push_back(LocationDirective(begin + 1, begin + 6, "/"));
	*advanced_len = 9;
}
