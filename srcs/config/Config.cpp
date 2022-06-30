#include "Config.hpp"

Config::Config() :
	servers_()
{
}

Config::~Config()
{
	std::vector<ServerDirective*>::iterator	it = servers_.begin();
	std::vector<ServerDirective*>::iterator	end = servers_.end();
	while (it != end)
	{
		delete *it;
		++it;
	}
}

void	Config::ParseConfigFile()
{
	ServerDirective	*new_server = new ServerDirective();
	new_server->ParseServerDirective();
	servers_.push_back(new_server);
}
