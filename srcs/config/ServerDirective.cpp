#include "ServerDirective.hpp"

ServerDirective::ServerDirective() :
	listen_("*", 80), server_names_(), error_pages_(), client_max_body_size_(1048576)
{
	server_names_.push_back("");
}

ServerDirective::~ServerDirective()
{
	std::vector<LocationDirective*>::iterator	it = locations_.begin();
	std::vector<LocationDirective*>::iterator	end = locations_.end();
	while (it != end)
	{
		delete *it;
		++it;
	}
}

void	ServerDirective::ParseServerDirective()
{
	LocationDirective	*new_location = new LocationDirective();
	new_location->ParseLocationDirective();
	locations_.push_back(new_location);
}
