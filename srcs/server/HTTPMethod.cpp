#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include "HTTPMethod.hpp"

HTTPMethod::HTTPMethod()
{
}

HTTPMethod::~HTTPMethod()
{
}

const std::string&	HTTPMethod::GetContentType() const	{ return (content_type_); }
const std::string&	HTTPMethod::GetLocation()	 const	{ return (location_); }
const std::string&	HTTPMethod::GetBody()		 const	{ return (body_); }

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

bool	HTTPMethod::CheckSlashEnd
	(const std::string& target, const std::string& hostname, const int port)
{
	if (*(target.rbegin()) == '/')
		return (true);

	std::stringstream	ss;
	ss << port;
	location_ = "http://" + hostname + ":" + ss.str() + target + "/";
	return (false);
}

bool	HTTPMethod::GetFile(const std::string& file_path)
{
	std::ifstream ifs(file_path);
	if (ifs.fail())
		return (false);
	std::istreambuf_iterator<char> itr(ifs);
	std::istreambuf_iterator<char> last;
	body_.assign(itr, last);
	return (true);
}

bool	HTTPMethod::GetFileWithIndex
	(const std::string& access_path, const std::vector<std::string>& indexes)
{
	std::vector<std::string>::const_iterator	itr = indexes.begin();
	std::vector<std::string>::const_iterator	end = indexes.end();

	while (itr != end)
	{
		if (GetFile(access_path + *itr))
			return (true);
		++itr;
	}
	return (false);
}

// bool	HTTPMethod::GetAutoIndexFile(const bool autoindex)
// {
// 	if (autoindex == false)
// 		return (false);
// 	body_ = "autoindex";
// 	return (true);
// }

int		HTTPMethod::ExecGETMethod
	(const LocationDirective& location, const std::string& access_path, const struct stat& st)
{
	if (S_ISREG(st.st_mode))
	{
		if (GetFile(access_path))
			return (200);
		throw HTTPError(HTTPError::FORBIDDEN);
	}
	else if (S_ISDIR(st.st_mode))
	{
		if (!CheckSlashEnd(req_->GetTarget(), req_->GetHost().first, server_conf_->GetListen().second))
			return (301);
		else if (GetFileWithIndex(access_path, location.GetIndex()))
			return (200);
		// else if (GetAutoIndexFile(location.GetAutoIndex()))
		// 	return (200);
		throw HTTPError(HTTPError::FORBIDDEN);
	}
	else
		throw HTTPError(HTTPError::FORBIDDEN);
}

int		HTTPMethod::ExecDELETEMethod
	(const LocationDirective& location, const std::string& access_path, const struct stat& st)
{
	if (S_ISDIR(st.st_mode) && *(req_->GetTarget().rbegin()) != '/')
		throw HTTPError(HTTPError::CONFLICT);

	if (unlink(access_path.c_str()) == -1)
		throw HTTPError(HTTPError::FORBIDDEN);

	return (200);
}

int		HTTPMethod::ExecHTTPMethod(const HTTPRequest& req, const ServerDirective& server_conf)
{
	req_ = &req;
	server_conf_ = &server_conf;

	const LocationDirective&			location = SelectLocation(req.GetTarget(), server_conf.GetLocations());
	const std::pair<int, std::string>&	redirect = location.GetReturn();
	if (redirect.first != -1)
	{
		location_ = redirect.second;
		return (redirect.first);
	}

	const std::vector<std::string>&	allow_methods = location.GetAllowedMethods();
	const std::string&				method = req.GetMethod();
	if (std::find(allow_methods.begin(), allow_methods.end(), method) == allow_methods.end())
		throw HTTPError(HTTPError::METHOD_NOT_ALLOWED);

	const std::string	access_path = location.GetRoot() + req.GetTarget();
	struct stat			st;
	if (stat(access_path.c_str(), &st) == -1)
		throw HTTPError(HTTPError::NOT_FOUND);
	if (method == "GET")
		return (ExecGETMethod(location, access_path, st));
	else if (method == "DELETE")
		return (ExecDELETEMethod(location, access_path, st));
	// else if (method == "POST")
	// 	return (ExecDELETEMethod(location, access_path, st));
}

void	HTTPMethod::MethodDisplay() const
{
	std::cout << "content_type: " << content_type_ << std::endl;
	std::cout << "location: " << location_ << std::endl;
	std::cout << "[body]\n" << body_ << std::endl;
}
