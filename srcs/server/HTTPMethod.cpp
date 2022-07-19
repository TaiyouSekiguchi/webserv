#include <fstream>
#include <sstream>
#include "HTTPMethod.hpp"
#include "Dir.hpp"

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
	(const std::vector<LocationDirective>& locations) const
{
	std::vector<LocationDirective>::const_iterator	itr = locations.begin();
	std::vector<LocationDirective>::const_iterator	end = locations.end();
	std::vector<LocationDirective>::const_iterator 	longest = end;
	const std::string& 								target = req_->GetTarget();

	while (itr != end)
	{
		if (target.find(itr->GetPath()) != std::string::npos)
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

int		HTTPMethod::Redirect(const std::string& location, const int status_code)
{
	location_ = location;
	return (status_code);
}

bool	HTTPMethod::GetFile(const std::string& access_path)
{
	std::ifstream ifs(access_path);
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

bool	HTTPMethod::GetAutoIndexFile(const std::string& access_path, const bool autoindex)
{
	if (autoindex == false)
		return (false);

	std::stringstream	body_stream;
	std::string			name;
	body_stream
		<< "<html>\r\n" << "<head><title>Index of /</title></head>\r\n"
		<< "<body>\r\n" << "<h1>Index of /</h1><hr><pre><a href=\"../\">../</a>\r\n";

	Dir		dir(access_path);
	if (dir.Fail())
		throw HTTPError(HTTPError::INTERNAL_SERVER_ERROR);
	while ((name = dir.GetValidFileName()).size() != 0)
	{
		Stat st(access_path + "/" + name);
		if (st.Fail())
			throw HTTPError(HTTPError::INTERNAL_SERVER_ERROR);
		body_stream
			<< "<a href=\"" << name << "\">" << name << "</a>\t\t"
			<< st.GetModifyTime() << "\t" << st.GetSize() << "\r\n";
	}

	body_stream << "</pre><hr></body>\r\n" << "</html>\r\n";
	body_ = body_stream.str();
	return (true);
}

int		HTTPMethod::ExecGETMethod(const Stat& st, const LocationDirective& location)
{
	const std::string&	access_path = st.GetPath();
	std::string			host;
	std::string			ip;

	if (st.IsRegularFile())
	{
		if (GetFile(access_path))
			return (200);
		throw HTTPError(HTTPError::FORBIDDEN);
	}
	else if (st.IsDirectory())
	{
		if (*(req_->GetTarget().rbegin()) != '/')
		{
			host = req_->GetHost().first;
			ip = Utils::toString(server_conf_->GetListen().second);
			return (Redirect("http://" + host + ":" + ip + req_->GetTarget() + "/", 301));
		}
		else if (GetFileWithIndex(access_path, location.GetIndex()))
			return (200);
		else if (GetAutoIndexFile(access_path, location.GetAutoIndex()))
			return (200);
		throw HTTPError(HTTPError::FORBIDDEN);
	}
	else
		throw HTTPError(HTTPError::FORBIDDEN);
}

int		HTTPMethod::ExecDELETEMethod(const Stat& st)
{
	if (st.IsDirectory() && *(req_->GetTarget().rbegin()) != '/')
		throw HTTPError(HTTPError::CONFLICT);

	if (std::remove(st.GetPath().c_str()) == -1)
	{
		if (errno == EACCES || errno == ENOTEMPTY)
			throw HTTPError(HTTPError::FORBIDDEN);
		else
			throw HTTPError(HTTPError::INTERNAL_SERVER_ERROR);
	}
	return (204);
}

int		HTTPMethod::ExecPOSTMethod(const Stat& st)
{
	if (!st.IsDirectory())
		throw HTTPError(HTTPError::CONFLICT);

	std::fstream		output_fstream;
	const std::string	timestamp = Utils::toString(time(NULL));
	const std::string	file_path = st.GetPath() + "/" + timestamp;

	Stat	check_st(file_path);
	if (!check_st.Fail())
		throw HTTPError(HTTPError::CONFLICT);
    output_fstream.open(file_path, std::ios_base::out);
    if (!output_fstream.is_open())
		throw HTTPError(HTTPError::INTERNAL_SERVER_ERROR);
	output_fstream << req_->GetBody();
	if (*(req_->GetTarget().rbegin()) == '/')
		location_ = req_->GetTarget() + timestamp;
	else
		location_ = req_->GetTarget() + "/" + timestamp;
	return (201);
}

bool	HTTPMethod::CheckCGIScript(const Stat& st, const LocationDirective& location)
{
	if (st.Fail() || !st.IsRegularFile())
		return (false);

	std::string::size_type	dot_pos;
	std::string				extension;
	const std::string&		access_path = st.GetPath();

	dot_pos = access_path.find_last_of('.');
	if (dot_pos == std::string::npos || dot_pos + 1 == access_path.size())
		return (false);
	extension = access_path.substr(dot_pos + 1);
	if (Utils::isNotFound(location.GetCGIEnableExtension(), extension))
		return (false);
	return (true);
}

// int		HTTPMethod::ExecCGI(const std::string& access_path)
// {
// 	CGI cgi(access_path);
// 	body_ = cgi.GetBody();
// 	location_ = cgi.GetLocation();
// 	content_type_ = cgi.GetContentType();
// 	return (cgi.GetStatusCode());
// }

int		HTTPMethod::SwitchHTTPMethod(const LocationDirective& location)
{
	const std::string&	method = req_->GetMethod();

	std::string			access_path;
	if (method == "POST")
		access_path = location.GetUploadRoot() + req_->GetTarget();
	else
		access_path = location.GetRoot() + req_->GetTarget();

	Stat	st(access_path);
	if (st.Fail())
		throw HTTPError(HTTPError::NOT_FOUND);

	if (method == "GET")
		return (ExecGETMethod(st, location));
	else if (method == "DELETE")
		return (ExecDELETEMethod(st));
	else
		return (ExecPOSTMethod(st));
}

int		HTTPMethod::ExecHTTPMethod(const HTTPRequest& req, const ServerDirective& server_conf)
{
	req_ = &req;
	server_conf_ = &server_conf;
	const LocationDirective&	location = SelectLocation(server_conf.GetLocations());

	const std::pair<int, std::string>&	redirect = location.GetReturn();
	if (redirect.first != -1)
		return (Redirect(redirect.second, redirect.first));

	if (Utils::isNotFound(location.GetAllowedMethods(), req.GetMethod()))
		throw HTTPError(HTTPError::METHOD_NOT_ALLOWED);

	Stat	cgi_st(location.GetRoot() + req.GetTarget());
	if (CheckCGIScript(cgi_st, location))
		return (200);
		// return (ExecCGI(cgi_st.GetPath()));

	return (SwitchHTTPMethod(location));
}

void	HTTPMethod::MethodDisplay() const
{
	std::cout << "content_type: " << content_type_ << std::endl;
	std::cout << "location: " << location_ << std::endl;
	std::cout << "[body]\n" << body_ << std::endl;
}
