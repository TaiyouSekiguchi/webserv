#include <map>
#include "LocationDirective.hpp"

LocationDirective::LocationDirective(const std::string& path, Tokens::citr begin, Tokens::citr end)
	: path_(path)
{
	const std::pair<std::string, SetFunc> p[] = {
		std::make_pair("index", &LocationDirective::SetIndex),
		std::make_pair("alias",  &LocationDirective::SetAlias)
	};
	const std::map<std::string, SetFunc>			set_funcs(p, &p[2]);
	std::map<std::string, SetFunc>::const_iterator	found;
	Tokens::citr									itr;
	Tokens::citr									directive_end;

	SetDefaultValues();

	itr = begin;
	while (itr < end)
	{
		found = set_funcs.find(*itr);
		if (found == set_funcs.end())
			throw std::runtime_error("conf syntax error");
		directive_end = GetDirectiveEnd(*itr, itr + 1, end);
		if (directive_end == end)
			throw std::runtime_error("conf syntax error");
		(this->*(found->second))(itr + 1, end);
		itr = directive_end + 1;
	}
}

LocationDirective::~LocationDirective()
{
}

const std::string&				LocationDirective::GetPath() const { return (path_); }
const std::string&				LocationDirective::GetAlias() const { return (alias_); }
const std::vector<std::string>&	LocationDirective::GetIndex() const { return (index_); }

Tokens::citr	LocationDirective::GetDirectiveEnd
	(const std::string& name, Tokens::citr begin, Tokens::citr end) const
{
	Tokens::citr	directive_end;

	(void)name;
	directive_end = std::find(begin, end, ";");
	return (directive_end);
}

void	LocationDirective::SetDefaultValues()
{
	// index_.push_back("index.html");
	// autoindex_ = false;
}

void	LocationDirective::SetAlias(Tokens::citr begin, Tokens::citr end)
{
	(void)begin;
	(void)end;
	alias_ = *begin;
}

void	LocationDirective::SetIndex(Tokens::citr begin, Tokens::citr end)
{
	(void)begin;
	(void)end;
	index_.push_back(*begin);
}
