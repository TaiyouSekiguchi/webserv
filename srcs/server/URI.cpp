#include "URI.hpp"

URI::URI(const std::string& root, const std::string& target)
	: root_(root)
	, target_(target)
{
	std::string::size_type	pos;

	pos = target_.find("?");
	if (pos == std::string::npos)
		path_ = target_;
	else
	{
		path_ = target_.substr(0, pos);
		query_ = target_.substr(pos + 1);
	}

	full_path_ = root_ + path_;
}

URI::~URI()
{
}

const std::string&		URI::GetFullPath(void) const { return (full_path_) ; }
const std::string&		URI::GetPath(void) const { return (path_) ; }
const std::string&		URI::GetQuery(void) const { return (query_) ; }
