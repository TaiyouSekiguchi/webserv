#include <fstream>
#include <stdexcept>
#include "Config.hpp"

Config::Config(const std::string& file_path)
	: tokens_(file_path)
{
	Tokens::citr	itr = tokens_.begin();
	Tokens::citr	tokens_end = tokens_.end();
	int				advanced_len;

	while (itr != tokens_end)
	{
		if (*itr == "server")
			SetServer(++itr, tokens_end, &advanced_len);
		else
			throw std::runtime_error("conf syntax error");
		itr += advanced_len;
	}
}

Config::~Config()
{
}

const std::vector<ServerDirective>&	Config::GetServers() const { return (servers_); }

void	Config::SetServer(Tokens::citr begin, Tokens::citr tokens_end, int *advaced_len)
{
	if (begin == tokens_end || *begin != "{")
		throw std::runtime_error("conf syntax error");

	Tokens::citr	server_begin_itr = begin + 1;
	Tokens::citr	end_braces_itr = Tokens::GetEndBracesItr(server_begin_itr, tokens_end);
	if (end_braces_itr == tokens_end)
		throw std::runtime_error("conf syntax error");

	*advaced_len = end_braces_itr - begin + 1;
	servers_.push_back(ServerDirective(server_begin_itr, end_braces_itr));
}
