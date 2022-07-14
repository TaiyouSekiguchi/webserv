#include <map>
#include "LocationDirective.hpp"

LocationDirective::LocationDirective()
	: path_("/")
{
	SetDefaultValues();
}

LocationDirective::LocationDirective(const std::string& path, Tokens::citr begin, Tokens::citr end)
	: path_(path)
{
	const std::pair<std::string, ParseFunc> p[] = {
		std::make_pair("root", &LocationDirective::ParseRoot),
		std::make_pair("index", &LocationDirective::ParseIndex),
		std::make_pair("return", &LocationDirective::ParseReturn),
		std::make_pair("autoindex", &LocationDirective::ParseAutoIndex),
		std::make_pair("allowed_methods", &LocationDirective::ParseAllowedMethods)
	};
	const std::map<std::string, ParseFunc>				parse_funcs(p, &p[5]);
	std::map<std::string, ParseFunc>::const_iterator	found;
	Tokens::citr										itr;
	Tokens::citr										directive_end;

	SetDefaultValues();

	itr = begin;
	while (itr < end)
	{
		found = parse_funcs.find(*itr);
		if (found == parse_funcs.end())
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

const std::string&					LocationDirective::GetPath() const { return (path_); }
const std::string&					LocationDirective::GetRoot() const { return (root_); }
const std::vector<std::string>&		LocationDirective::GetIndex() const { return (index_); }
const std::pair<int, std::string>&	LocationDirective::GetReturn() const { return (return_); }
const bool&							LocationDirective::GetAutoIndex() const { return (autoindex_); }
const std::vector<std::string>&		LocationDirective::GetAllowedMethods() const { return (allowed_methods_); }

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
	return_ = std::make_pair(-1, "");
	autoindex_ = false;
	allowed_methods_.push_back("GET");
}

void	LocationDirective::ParseRoot(Tokens::citr begin, Tokens::citr end)
{
	if (begin + 1 != end)
		throw std::runtime_error("conf syntax error");
	else if (Tokens::isSpecialToken(*begin))
		throw std::runtime_error("conf syntax error");
	root_ = *begin;
}

void	LocationDirective::ParseIndex(Tokens::citr begin, Tokens::citr end)
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

void	LocationDirective::ParseReturn(Tokens::citr begin, Tokens::citr end)
{
	if (begin + 1 != end && begin + 2 != end)
		throw std::runtime_error("conf syntax error");

	bool			is_url;
	char			*endptr;
	long			status_code = 302;
	std::string		url = "";

	is_url = (*begin).find("http://") != std::string::npos;
	is_url |= (*begin).find("https://") != std::string::npos;
	if (begin + 2 == end || !is_url)
	{
		status_code = std::strtol((*begin).c_str(), &endptr, 10);
		if (*endptr != '\0' || errno == ERANGE || status_code < 1 || 999 < status_code)
			throw std::runtime_error("conf syntax error");
	}
	if (begin + 2 == end || is_url)
	{
		if (begin + 1 == end)
			url = *begin;
		else
			url = *(begin + 1);
		if (Tokens::isSpecialToken(url))
			throw std::runtime_error("conf syntax error");
	}
	return_ = std::make_pair(status_code, url);
}

void	LocationDirective::ParseAutoIndex(Tokens::citr begin, Tokens::citr end)
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

void	LocationDirective::ParseAllowedMethods(Tokens::citr begin, Tokens::citr end)
{
	Tokens::citr								itr;
	std::vector<std::string>::const_iterator	found;

	itr = begin;
	while (itr != end)
	{
		if (*itr != "GET" && *itr != "POST" && *itr != "DELETE")
			throw std::runtime_error("conf syntax error");
		found = std::find(allowed_methods_.begin(), allowed_methods_.end(), *itr);
		if (found == allowed_methods_.end())
			allowed_methods_.push_back(*itr);
		itr++;
	}
}
