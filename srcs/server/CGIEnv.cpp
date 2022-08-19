#include "CGIEnv.hpp"

CGIEnv::CGIEnv(const URI& uri, const HTTPRequest& req)
	: uri_(uri)
	, req_(req)
	, server_conf_(req.GetServerConf())
	, exec_env_(NULL)
{
	SetEnv();
}

CGIEnv::~CGIEnv(void)
{
	if (exec_env_ != NULL)
		delete [] exec_env_;
}

void	CGIEnv::AddEnv(const std::string& key, const std::string& value)
{
	env_.push_back(key + "=" + value);
}

void	CGIEnv::SetEnv(void)
{
	AddEnv("AUTH_TYPE", req_.GetAuthorization().first);
	AddEnv("CONTENT_LENGTH", req_.GetMethod() == "POST" ? Utils::ToString(req_.GetContentLength()) : "");
	AddEnv("CONTENT_TYPE", req_.GetContentType());
	AddEnv("GATEWAY_INTERFACE", "CGI/1.1");
	AddEnv("HTTP_ACCEPT", req_.GetAccept());
	AddEnv("HTTP_REFERER", "");
	AddEnv("HTTP_USER_AGENT", req_.GetUserAgent());
	AddEnv("PATH_INFO", uri_.GetTargetPath());
	AddEnv("PATH_TRANSLATED", uri_.GetAccessPath());
	AddEnv("QUERY_STRING", uri_.GetQuery());
	AddEnv("REMOTE_ADDR", "");
	AddEnv("REMOTE_HOST", "");
	AddEnv("REMOTE_IDENT", "");
	AddEnv("REMOTE_USER", "");
	AddEnv("REQUEST_METHOD", req_.GetMethod());
	AddEnv("SCRIPT_NAME", uri_.GetTargetPath());
	AddEnv("SERVER_NAME", server_conf_->GetServerNames().at(0));
	AddEnv("SERVER_PORT", Utils::ToString(req_.GetListen().second));
	AddEnv("SERVER_PROTOCOL", "HTTP/1.1");
	AddEnv("SERVER_SOFTWARE", "42Webserv");
}

char**	CGIEnv::GetEnv(void)
{
	size_t		size;

	size = env_.size();
	exec_env_ = new char*[size + 1];
	for (size_t i = 0; i < size; i++)
		exec_env_[i] = const_cast<char *>(env_[i].c_str());
	exec_env_[size] = NULL;

	return (exec_env_);
}
