#include <map>
#include "LocationDirective.hpp"

LocationDirective::LocationDirective(
	Tokens::citr begin, Tokens::citr location_end, const std::string& path)
	: path_(path)
{
	const std::pair<std::string, SetFunc> p[] = {
		std::make_pair("index", &LocationDirective::SetIndex),
		std::make_pair("root",  &LocationDirective::SetRoot)
	};
	const std::map<std::string, SetFunc>			set_funcs(p, &p[2]);
	std::map<std::string, SetFunc>::const_iterator	found;

	Tokens::citr	itr = begin;
	int				advanced_len;

	InitDirectives();

	while (itr != location_end)
	{
		found = set_funcs.find(*itr);
		if (found == set_funcs.end())
			throw std::runtime_error("conf syntax error");
		(this->*(found->second))(++itr, location_end, &advanced_len);
		itr += advanced_len;
	}
}

LocationDirective::~LocationDirective()
{
}

const std::string&				LocationDirective::GetPath() const { return (path_); }
const std::string&				LocationDirective::GetRoot() const { return (root_); }
const std::vector<std::string>&	LocationDirective::GetIndex() const { return (index_); }

void	LocationDirective::InitDirectives()
{
	root_ = "html";
	index_.push_back("index.html");
	// autoindex_ = false;
}

void	LocationDirective::SetRoot(Tokens::citr begin, Tokens::citr location_end, int *advanced_len)
{
	(void)begin;
	(void)location_end;
	root_ = "html";
	*advanced_len = 3;
}

void	LocationDirective::SetIndex(Tokens::citr begin, Tokens::citr location_end, int *advanced_len)
{
	(void)begin;
	(void)location_end;
	index_.push_back("index.html");
	*advanced_len = 3;
}
