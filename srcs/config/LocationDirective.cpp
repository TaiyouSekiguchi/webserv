#include <map>
#include "LocationDirective.hpp"

LocationDirective::LocationDirective(const std::string& path, Tokens::citr begin, Tokens::citr end)
	: path_(path)
{
	const std::pair<std::string, SetFunc> p[] = {
		std::make_pair("index", &LocationDirective::SetIndex),
		std::make_pair("root", &LocationDirective::SetRoot),
		std::make_pair("autoindex", &LocationDirective::SetAutoIndex),
		std::make_pair("allowed_methods", &LocationDirective::SetAllowedMethods)
	};
	const std::map<std::string, SetFunc>			set_funcs(p, &p[4]);
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
		if (directive_end == end || itr + 1 == directive_end)
			throw std::runtime_error("conf syntax error");
		(this->*(found->second))(itr + 1, directive_end);
		itr = directive_end + 1;
	}
}

LocationDirective::~LocationDirective()
{
}

const std::string&				LocationDirective::GetPath() const { return (path_); }
const std::string&				LocationDirective::GetRoot() const { return (root_); }
const std::vector<std::string>&	LocationDirective::GetIndex() const { return (index_); }
const bool&						LocationDirective::GetAutoIndex() const { return (autoindex_); }
const std::vector<std::string>&	LocationDirective::GetAllowedMethods() const { return (allowed_methods_); }

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
	root_ = "html";
	index_.push_back("index.html");
	autoindex_ = false;
	allowed_methods_.push_back("GET");
	allowed_methods_.push_back("POST");
	allowed_methods_.push_back("DELETE");
}

void	LocationDirective::SetRoot(Tokens::citr begin, Tokens::citr end)
{
	if (begin + 1 != end)
		throw std::runtime_error("conf syntax error");
	else if (Tokens::isSpecialToken(*begin))
		throw std::runtime_error("conf syntax error");
	root_ = *begin;
}

void	LocationDirective::SetIndex(Tokens::citr begin, Tokens::citr end)
{
	index_.clear();

	Tokens::citr	itr = begin;
	while (itr != end)
	{
		if (Tokens::isSpecialToken(*itr))
			throw std::runtime_error("conf syntax error");
		index_.push_back(*itr);
		itr++;
	}
}

void	LocationDirective::SetAutoIndex(Tokens::citr begin, Tokens::citr end)
{
	if (begin + 1 != end)
		throw std::runtime_error("conf syntax error");
	if (*begin == "on")
		autoindex_ = true;
	else if (*begin == "off")
		autoindex_ = false;
	else
		throw std::runtime_error("conf syntax error");
}

void	LocationDirective::SetAllowedMethods(Tokens::citr begin, Tokens::citr end)
{
	allowed_methods_.clear();

	Tokens::citr	itr = begin;
	while (itr != end)
	{
		if (*itr != "GET" && *itr != "POST" && *itr != "DELETE")
			throw std::runtime_error("conf syntax error");
		allowed_methods_.push_back(*itr);
		itr++;
	}
}
