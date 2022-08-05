#include "URI.hpp"

URI::URI(const std::string& root, const std::string& target)
{
	std::string::size_type	pos;

	pos = target.find("?");
	if (pos == std::string::npos)
		target_path_ = target;
	else
	{
		target_path_ = target.substr(0, pos);
		query_ = target.substr(pos + 1);
	}

	access_path_ = root + target_path_;
}

URI::~URI()
{
}

const std::string&		URI::GetAccessPath(void) const { return (access_path_) ; }
const std::string&		URI::GetTargetPath(void) const { return (target_path_) ; }
const std::string&		URI::GetQuery(void) const { return (query_) ; }
