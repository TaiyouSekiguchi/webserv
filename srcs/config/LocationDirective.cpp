#include "LocationDirective.hpp"

LocationDirective::LocationDirective() :
	return_(), root_("html"), autoindex_(false), indexes_()
{
	indexes_.push_back("index.html");
}

LocationDirective::~LocationDirective()
{
}

void	LocationDirective::ParseLocationDirective()
{
}
