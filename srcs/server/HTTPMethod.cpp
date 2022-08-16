#include <fstream>
#include <sstream>
#include "HTTPMethod.hpp"
#include "HTTPResponse.hpp"
#include "Dir.hpp"

HTTPMethod::HTTPMethod(const HTTPRequest& req)
	: req_(req), target_rfile_(NULL)
{
	headers_["Connection"] = req_.GetConnection() ? "keep-alive" : "close";
}

HTTPMethod::~HTTPMethod()
{
	if (target_rfile_)
		delete target_rfile_;
}

const e_StatusCode&					HTTPMethod::GetStatusCode()	const { return (status_code_); }
std::map<std::string, std::string>	HTTPMethod::GetHeaders()	const { return (headers_); }
const std::string&					HTTPMethod::GetBody()		const { return (body_); }

int		HTTPMethod::GetTargetFileFd() const { return (target_rfile_->GetFd()); }

void	HTTPMethod::ExecGETMethod()
{
	ssize_t		ret;
	std::string	body;

	ret = target_rfile_->ReadFile(&body);
	delete target_rfile_;
	target_rfile_ = NULL;
	if (ret == -1)
		throw HTTPError(SC_FORBIDDEN, "ExecGETMethod");

	status_code_ = SC_OK;
	body_ = body;
	headers_["Content-Length"] = Utils::ToString(body_.size());
}

void	HTTPMethod::ExecPOSTMethod()
{
	ssize_t				ret;
	const std::string	file_name = target_rfile_->GetName();

	ret = target_rfile_->WriteToFile(req_.GetBody());
	delete target_rfile_;
	target_rfile_ = NULL;
	if (ret == -1)
		throw HTTPError(SC_FORBIDDEN, "ExecPOSTMethod");

	if (*(req_.GetTarget().rbegin()) == '/')
		headers_["Location"] = req_.GetTarget() + file_name;
	else
		headers_["Location"] = req_.GetTarget() + "/" + file_name;
	status_code_ = SC_CREATED;
}

void	HTTPMethod::ExecDELETEMethod()
{
	int		ret;

	ret = target_rfile_->DeleteFile();
	delete target_rfile_;
	target_rfile_ = NULL;
	if (ret == -1)
		throw HTTPError(SC_FORBIDDEN, "ExecDELETEMethod");

	status_code_ = SC_NO_CONTENT;
}

LocationDirective	HTTPMethod::SelectLocation
	(const std::vector<LocationDirective>& locations) const
{
	std::vector<LocationDirective>::const_iterator	itr = locations.begin();
	std::vector<LocationDirective>::const_iterator	end = locations.end();
	std::vector<LocationDirective>::const_iterator 	longest = itr++;
	const std::string& 								target = req_.GetTarget();

	while (itr != end)
	{
		if (target.find(itr->GetPath()) != std::string::npos)
		{
			if (longest->GetPath().size() < itr->GetPath().size())
				longest = itr;
		}
		++itr;
	}
	return (*longest);
}

e_HTTPServerEventType	HTTPMethod::Redirect(const std::string& return_second, const e_StatusCode status_code)
{
	if (status_code == SC_MOVED_PERMANENTLY
		|| status_code == SC_FOUND
		|| status_code == SC_SEE_OTHER
		|| status_code == SC_TEMPORARY_REDIRECT)
	{
		headers_["Location"] = return_second;
		throw HTTPError(status_code, "Redirect");
	}
	status_code_ = status_code;
	body_ = return_second;
	headers_["Content-Length"] = Utils::ToString(body_.size());
	return (SEVENT_NO);
}

e_HTTPServerEventType	HTTPMethod::PublishReadEvent(const e_HTTPServerEventType event_type)
{
	if (target_rfile_->GetSize() == 0)
	{
		delete target_rfile_;
		target_rfile_ = NULL;
		status_code_ = SC_OK;
		headers_["Content-Length"] = "0";
		return (SEVENT_NO);
	}
	else
		return (event_type);
}

bool	HTTPMethod::IsReadableFile(const std::string& access_path)
{
	target_rfile_ = new RegularFile(access_path, O_RDONLY);
	if (target_rfile_->Fail())
	{
		delete target_rfile_;
		target_rfile_ = NULL;
		return (false);
	}
	return (true);
}

bool	HTTPMethod::IsReadableFileWithIndex
	(const std::string& access_path, const std::vector<std::string>& indexes)
{
	std::vector<std::string>::const_iterator	itr = indexes.begin();
	std::vector<std::string>::const_iterator	end = indexes.end();

	while (itr != end)
	{
		if (IsReadableFile(access_path + *itr))
			return (true);
		++itr;
	}
	return (false);
}

void	HTTPMethod::SetAutoIndexContent(const std::string& access_path)
{
	std::stringstream	body_stream;
	body_stream
		<< "<html>\r\n"
		<< "<head><title>Index of " << req_.GetTarget() << "</title></head>\r\n"
		<< "<body>\r\n" << "<h1>Index of /</h1><hr><pre><a href=\"../\">../</a>\r\n";

	Dir		dir(access_path);
	if (dir.Fail())
		throw HTTPError(SC_INTERNAL_SERVER_ERROR, "GetAutoIndexFile");

	const std::vector<std::string>&				names = dir.GetFileNameList();
	std::vector<std::string>::const_iterator	itr = names.begin();
	std::vector<std::string>::const_iterator	end = names.end();
	while (itr != end)
	{
		Stat st(access_path + *itr);
		if (st.Fail())
			throw HTTPError(SC_INTERNAL_SERVER_ERROR, "GetAutoIndexFile");
		body_stream
			<< "<a href=\"" << *itr << "\">" << *itr << "</a>\t\t"
			<< st.GetModifyTime() << "\t" << st.GetSizeStr() << "\r\n";
		++itr;
	}

	body_stream << "</pre><hr></body>\r\n" << "</html>\r\n";
	status_code_ = SC_OK;
	body_ = body_stream.str();
	headers_["Content-Length"] = Utils::ToString(body_.size());
}

e_HTTPServerEventType	HTTPMethod::ValidateGETMethod(const Stat& st, const LocationDirective& location)
{
	const std::string&	access_path = st.GetPath();

	if (st.IsRegularFile())
	{
		if (IsReadableFile(access_path))
			return (PublishReadEvent(SEVENT_FILE_READ));
		throw HTTPError(SC_FORBIDDEN, "ValidateGETMethod");
	}
	else if (st.IsDirectory())
	{
		if (*(req_.GetTarget().rbegin()) != '/')
		{
			const std::string& host = req_.GetHost().first;
			const std::string& ip = Utils::ToString(req_.GetListen().second);
			const std::string  location = "http://" + host + ":" + ip + req_.GetTarget() + "/";
			return (Redirect(location, SC_MOVED_PERMANENTLY));
		}
		else if (IsReadableFileWithIndex(access_path, location.GetIndex()))
			return (PublishReadEvent(SEVENT_FILE_READ));
		else if (location.GetAutoIndex())
		{
			SetAutoIndexContent(access_path);
			return (SEVENT_NO);
		}
		throw HTTPError(SC_FORBIDDEN, "ValidateGETMethod");
	}
	else
		throw HTTPError(SC_FORBIDDEN, "ValidateGETMethod");
}

e_HTTPServerEventType	HTTPMethod::ValidateDELETEMethod(const Stat& st)
{
	if (st.IsDirectory() && *(req_.GetTarget().rbegin()) != '/')
		throw HTTPError(SC_CONFLICT, "ValidateDELETEMethod");

	if (st.IsDirectory())
		target_rfile_ = new RegularFile(st.GetPath(), O_DIRECTORY);
	else
		target_rfile_ = new RegularFile(st.GetPath(), O_WRONLY);
	if (target_rfile_->Fail())
	{
		delete target_rfile_;
		target_rfile_ = NULL;
		throw HTTPError(SC_FORBIDDEN, "ValidateDELETEMethod");
	}
	return (SEVENT_FILE_DELETE);
}

e_HTTPServerEventType	HTTPMethod::ValidatePOSTMethod(const Stat& st)
{
	if (!st.IsDirectory())
		throw HTTPError(SC_CONFLICT, "ValidatePOSTMethod");

	std::fstream		output_fstream;
	const std::string&	timestamp = Utils::GetMicroSecondTime();
	const std::string	file_path = st.GetPath() + "/" + timestamp;

	target_rfile_ = new RegularFile(file_path, O_WRONLY | O_CREAT | O_EXCL);
	if (target_rfile_->Fail())
	{
		delete target_rfile_;
		target_rfile_ = NULL;
		if (errno == EEXIST)
			throw HTTPError(SC_CONFLICT, "ValidatePOSTMethod");
		else
			throw HTTPError(SC_FORBIDDEN, "ValidatePOSTMethod");
	}
	return (SEVENT_FILE_WRITE);
}

// bool	HTTPMethod::CheckCGIScript(const Stat& st, const LocationDirective& location)
// {
// 	if (st.Fail() || !st.IsRegularFile())
// 		return (false);

// 	std::string::size_type	dot_pos;
// 	std::string				extension;
// 	const std::string&		access_path = st.GetPath();

// 	dot_pos = access_path.find_last_of('.');
// 	if (dot_pos == std::string::npos || dot_pos + 1 == access_path.size())
// 		return (false);
// 	extension = access_path.substr(dot_pos + 1);
// 	if (Utils::IsNotFound(location.GetCGIEnableExtension(), extension))
// 		return (false);
// 	return (true);
// }

// int		HTTPMethod::ExecCGI(const std::string& access_path)
// {
// 	CGI cgi(access_path);
// 	body_ = cgi.GetBody();
// 	location_ = cgi.GetLocation();
// 	content_type_ = cgi.GetContentType();
// 	return (cgi.GetStatusCode());
// }

e_HTTPServerEventType	HTTPMethod::ValidateAnyMethod(const LocationDirective& location)
{
	const std::string&	method = req_.GetMethod();

	std::string			access_path;
	if (method == "POST")
		access_path = location.GetUploadRoot() + req_.GetTarget();
	else
		access_path = location.GetRoot() + req_.GetTarget();

	Stat	st(access_path);
	if (st.Fail())
		throw HTTPError(SC_NOT_FOUND, "ValidateAnyMethod");

	if (method == "GET")
		return (ValidateGETMethod(st, location));
	else if (method == "DELETE")
		return (ValidateDELETEMethod(st));
	else
		return (ValidatePOSTMethod(st));
}

e_HTTPServerEventType	HTTPMethod::ValidateHTTPMethod()
{
	server_conf_ = req_.GetServerConf();
	const LocationDirective&	location = SelectLocation(server_conf_->GetLocations());

	const std::pair<e_StatusCode, std::string>&	redirect = location.GetReturn();
	if (redirect.first != SC_INVALID)
		return (Redirect(redirect.second, redirect.first));

	if (Utils::IsNotFound(location.GetAllowedMethods(), req_.GetMethod()))
		throw HTTPError(SC_METHOD_NOT_ALLOWED, "ValidateHTTPMethod");

	// Stat	cgi_st(location.GetRoot() + req_.GetTarget());
	// if (CheckCGIScript(cgi_st, location))
	// 	return (OK);
		// return (ExecCGI(cgi_st.GetPath()));

	return (ValidateAnyMethod(location));
}

void	HTTPMethod::ReadErrorPage()
{
	int			ret;
	std::string	body;

	ret = target_rfile_->ReadFile(&body);
	delete target_rfile_;
	target_rfile_ = NULL;
	if (ret == -1)
	{
		status_code_ = SC_FORBIDDEN;
		body_ = GenerateDefaultHTML();
		headers_["Content-Length"] = Utils::ToString(body_.size());
		return;
	}
	body_ = body;
	headers_["Content-Length"] = Utils::ToString(body_.size());
}

std::string HTTPMethod::GenerateDefaultHTML() const
{
	std::stringstream ss;

	ss << "<html>\r\n";
	ss << "<head><title>" << status_code_ << " " << HTTPResponse::kStatusMsg_[status_code_] <<"</title></head>\r\n";
	ss << "<body>\r\n";
	ss << "<center><h1>" << status_code_ << " " << HTTPResponse::kStatusMsg_[status_code_] << "</h1></center>\r\n";
	ss << "<hr><center>" << "Webserv" << "</center>\r\n";
	ss << "</body>\r\n";
	ss << "</html>\r\n";

	return (ss.str());
}

bool	HTTPMethod::IsConnectionCloseStatus(const e_StatusCode status_code) const
{
	return (status_code == SC_BAD_REQUEST || status_code == SC_REQUEST_TIMEOUT
		|| status_code == SC_LENGTH_REQUIRED || status_code == SC_URI_TOO_LONG
		|| status_code == SC_INTERNAL_SERVER_ERROR || status_code == SC_NOT_IMPLEMENTED
		|| status_code == SC_SERVISE_UNAVAILABLE || status_code == SC_HTTP_VERSION_NOT_SUPPORTED);
}

e_HTTPServerEventType	HTTPMethod::ValidateErrorPage(const e_StatusCode status_code)
{
	server_conf_ = req_.GetServerConf();
	status_code_ = status_code;
	if (IsConnectionCloseStatus(status_code))
		headers_["Connection"] = "close";

	const std::map<e_StatusCode, std::string>& 			error_pages = server_conf_->GetErrorPages();
	std::map<e_StatusCode, std::string>::const_iterator	found = error_pages.find(status_code_);
	if (found != error_pages.end())
	{
		std::string		error_page_path = found->second;
		if (error_page_path.at(0) == '/')
		{
			Stat	st("." + error_page_path);
			if (st.Fail())
				status_code_ = SC_NOT_FOUND;
			else
			{
				target_rfile_ = new RegularFile(st.GetPath(), O_RDONLY);
				if (!target_rfile_->Fail())
					return (PublishReadEvent(SEVENT_ERRORPAGE_READ));
				delete target_rfile_;
				target_rfile_ = NULL;
				status_code_ = SC_FORBIDDEN;
			}
		}
		else
		{
			status_code_ = SC_FOUND;
			headers_["Location"] = error_page_path;
		}
	}
	body_ = GenerateDefaultHTML();
	headers_["Content-Length"] = Utils::ToString(body_.size());
	return (SEVENT_NO);
}

void	HTTPMethod::MethodDisplay()
{
	std::cout << "status_code: " << status_code_ << std::endl;
	std::cout << "Connection: " << headers_["Connection"] << std::endl;
	std::cout << "Content-Type: " << headers_["Content-Type"] << std::endl;
	std::cout << "Content-Length: " << headers_["Content-Length"] << std::endl;
	std::cout << "Location: " << headers_["Location"] << std::endl;
	std::cout << "[ Body ]\n" << body_ << std::endl;
}
