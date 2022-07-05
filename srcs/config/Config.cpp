#include <fstream>
#include <stdexcept>
#include "Config.hpp"

Config::Config(const std::string& file_path)
	: tokens_(file_path)
{
	Tokens::citr	itr = tokens_.begin();
	Tokens::citr	tokens_end = tokens_.end();
	Tokens::citr	directive_end;

	while (itr != tokens_end)
	{
		if (*itr != "server")
			throw std::runtime_error("conf syntax error");
		directive_end = Tokens::GetEndBracesItr(itr + 1, tokens_end);
		if (directive_end == tokens_end)
			throw std::runtime_error("conf syntax error");
		SetServer(itr + 1, directive_end);
		itr = directive_end + 1;
	}
}

Config::~Config()
{
}

const std::vector<ServerDirective>&	Config::GetServers() const { return (servers_); }

void	Config::SetServer(Tokens::citr begin, Tokens::citr end)
{
	if (begin == end || *begin != "{")
		throw std::runtime_error("conf syntax error");
	servers_.push_back(ServerDirective(begin + 1, end));
}
