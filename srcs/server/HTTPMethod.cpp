#include <fstream>
#include "HTTPMethod.hpp"

HTTPMethod::HTTPMethod()
{
}

HTTPMethod::~HTTPMethod()
{
}

LocationDirective	HTTPMethod::SelectLocation
	(const std::string& target, const std::vector<LocationDirective>& locations) const
{
	std::vector<LocationDirective>::const_iterator	itr = locations.begin();
	std::vector<LocationDirective>::const_iterator	end = locations.end();
	std::vector<LocationDirective>::const_iterator 	longest = end;
	std::string::size_type							pos;

	while (itr != end)
	{
		pos = target.find(itr->GetPath());
		if (pos != std::string::npos)
		{
			if (longest == end || longest->GetPath().size() < itr->GetPath().size())
				longest = itr;
		}
		++itr;
	}
	if (longest == end)
		return (LocationDirective());
	return (*longest);
}

int		HTTPMethod::ExecHTTPMethod(const HTTPRequest& req, const ServerDirective& server_conf)
{
	const LocationDirective& location = SelectLocation(req.GetTarget(), server_conf.GetLocations());
	const std::string		access_path = location.GetRoot() + req.GetTarget();

	std::cout << "access_path: " << access_path << std::endl;
	std::ifstream ifs(access_path);
	if (ifs.fail())
		throw HTTPError(404);
	std::istreambuf_iterator<char> itr(ifs);
	std::istreambuf_iterator<char> last;
	body_.assign(itr, last);
	std::cout << "body: " << body_ << std::endl;
	return (200);
}
