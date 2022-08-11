#include <fstream>
#include <sstream>
#include "HTTPMethod.hpp"
#include "HTTPResponse.hpp"
#include "Dir.hpp"

HTTPMethod::HTTPMethod(const HTTPRequest& req)
	: req_(req), target_rfile_(NULL), uri_(NULL), cgi_(NULL)
{
}

HTTPMethod::~HTTPMethod()
{
	if (target_rfile_)
		delete target_rfile_;
	if (uri_)
		delete uri_;
	if (cgi_)
		delete cgi_;
}

const std::string&	HTTPMethod::GetContentType() const	{ return (content_type_); }
const std::string&	HTTPMethod::GetLocation()	 const	{ return (location_); }
const std::string&	HTTPMethod::GetBody()		 const	{ return (body_); }
const e_StatusCode&	HTTPMethod::GetStatusCode()	 const	{ return (status_code_); }

int		HTTPMethod::GetTargetFileFd() const { return (target_rfile_->GetFd()); }
// int		HTTPMethod::GetCgiReadPipeFd() const { return (cgi_->GetReadPipeFd()); }
int		HTTPMethod::GetCgiReadPipeFd() const { return (0); }
// int		HTTPMethod::GetCgiWritePipeFd() const { return (cgi_->GetWritePipeFd()); }
int		HTTPMethod::GetCgiWritePipeFd() const { return (0); }

void	HTTPMethod::ExecGETMethod()
{
	ssize_t		ret;
	std::string	body;

	ret = target_rfile_->ReadFile(&body);
	delete target_rfile_;
	target_rfile_ = NULL;
	if (ret == -1)
		throw HTTPError(SC_FORBIDDEN, "ExecGETMethod");

	body_ = body;
	status_code_ = SC_OK;
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

	if (*(uri_->GetTargetPath().rbegin()) == '/')
		location_ = uri_->GetTargetPath() + file_name;
	else
		location_ = uri_->GetTargetPath() + "/" + file_name;

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

void	HTTPMethod::PostToCgi()
{
	cgi_->PostToCgi();
}

e_HTTPServerEventType	HTTPMethod::ReceiveCgiResult(const bool eof_flag)
{
	e_HTTPServerEventType	event_type;

	event_type = cgi_->ReceiveCgiResult(eof_flag);
	if (event_type == SEVENT_NO)
	{
		body_ = cgi_->GetBody();
		location_ = cgi_->GetLocation();
		content_type_ = cgi_->GetContentType();
		status_code_ = cgi_->GetStatusCode();
	}
	return (event_type);
}

const LocationDirective*	HTTPMethod::SelectLocation
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
	return (&(*longest));
}

e_StatusCode	HTTPMethod::Redirect(const std::string& location, const e_StatusCode status_code)
{
	location_ = location;
	return (status_code);
}

e_HTTPServerEventType	HTTPMethod::PublishReadEvent(const e_HTTPServerEventType event_type)
{
	if (target_rfile_->GetSize() == 0)
	{
		delete target_rfile_;
		target_rfile_ = NULL;
		status_code_ = SC_OK;
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
		<< "<head><title>Index of " << uri_->GetTargetPath() << "</title></head>\r\n"
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
	body_ = body_stream.str();
	status_code_ = SC_OK;
}

e_HTTPServerEventType	HTTPMethod::ValidateGETMethod(const Stat& st)
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
		if (*(uri_->GetTargetPath().rbegin()) != '/')
		{
			const std::string& host = req_.GetHost().first;
			const std::string& ip = Utils::ToString(req_.GetListen().second);
			const std::string  location = "http://" + host + ":" + ip + uri_->GetTargetPath() + "/";
			throw HTTPError(Redirect(location, SC_MOVED_PERMANENTLY), "ValidateGETMethod");
		}
		else if (IsReadableFileWithIndex(access_path, location_conf_->GetIndex()))
			return (SEVENT_FILE_READ);
		else if (location_conf_->GetAutoIndex())
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
	if (st.IsDirectory() && *(uri_->GetTargetPath().rbegin()) != '/')
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

 bool	HTTPMethod::CheckCGIScript(void)
{
	Stat	st(uri_->GetAccessPath());

	if (st.Fail() || !st.IsRegularFile())
		return (false);

	std::string::size_type	dot_pos;
	std::string				extension;
	const std::string&		access_path = st.GetPath();

	dot_pos = access_path.find_last_of('.');
	if (dot_pos == std::string::npos || dot_pos + 1 == access_path.size())
		return (false);
	extension = access_path.substr(dot_pos + 1);
	if (Utils::IsNotFound(location_conf_->GetCGIEnableExtension(), extension))
		return (false);
	return (true);
}

e_HTTPServerEventType	HTTPMethod::ExecCGI(const std::string& access_path)
{
	cgi_ = new CGI(uri_, req_);

	return (cgi_->ExecCGI());
}

e_HTTPServerEventType	HTTPMethod::ValidateAnyMethod(void)
{
	const std::string&	method = req_.GetMethod();

	std::string			access_path;
	if (method == "POST")
		access_path = uri_->GetUploadAccessPath();
	else
		access_path = uri_->GetAccessPath();

	Stat	st(access_path);
	if (st.Fail())
		throw HTTPError(SC_NOT_FOUND, "ValidateAnyMethod");

	if (method == "GET")
		return (ValidateGETMethod(st));
	else if (method == "DELETE")
		return (ValidateDELETEMethod(st));
	else
		return (ValidatePOSTMethod(st));
}

e_HTTPServerEventType	HTTPMethod::ValidateHTTPMethod()
{
	server_conf_ = req_.GetServerConf();
	location_conf_ = SelectLocation(server_conf_->GetLocations());

	const std::pair<e_StatusCode, std::string>&	redirect = location_conf_->GetReturn();
	if (redirect.first != SC_INVALID)
		throw HTTPError(Redirect(redirect.second, redirect.first), "ValidateHTTPMethod");

	if (Utils::IsNotFound(location_conf_->GetAllowedMethods(), req_.GetMethod()))
		throw HTTPError(SC_METHOD_NOT_ALLOWED, "ValidateHTTPMethod");

	uri_ = new URI(*location_conf_, req_.GetTarget());

	if (CheckCGIScript())
		return (ExecCGI());

	return (ValidateAnyMethod());
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
		return;
	}
	body_ = body;
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

e_HTTPServerEventType	HTTPMethod::ValidateErrorPage(const e_StatusCode status_code)
{
	server_conf_ = req_.GetServerConf();
	status_code_ = status_code;

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
			location_ = error_page_path;
			status_code_ = SC_FOUND;
		}
	}
	body_ = GenerateDefaultHTML();
	return (SEVENT_NO);
}

void	HTTPMethod::MethodDisplay() const
{
	std::cout << "status_code: " << status_code_ << std::endl;
	std::cout << "content_type: " << content_type_ << std::endl;
	std::cout << "location: " << location_ << std::endl;
	std::cout << "[ Body ]\n" << body_ << std::endl;
}
