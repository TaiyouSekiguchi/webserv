#include "CGIEnv.hpp"

CGIEnv::CGIEnv(const URI& uri, const HTTPRequest& req)
	: uri_(uri)
	, req_(req)
	, server_conf_(req.GetServerConf())
{
	SetEnv();
}

CGIEnv::~CGIEnv(void)
{
}

void	CGIEnv::AddEnv(const std::string& key, const std::string& value)
{
	env_.push_back(key + "=" + value);
}

void	CGIEnv::SetEnv(void)
{
	AddEnv("AUTH_TYPE", "TEST");
	if (req_.GetContentLength() == 0)
		AddEnv("CONTENT_LENGTH", "");
	else
		AddEnv("CONTENT_LENGTH", Utils::ToString(req_.GetContentLength()));
	AddEnv("CONTENT_TYPE", req_.GetContentType());
	AddEnv("GATEWAY_INTERFACE", "CGI/1.1");
	AddEnv("HTTP_ACCEPT", "TEST");
	AddEnv("HTTP_REFERER", "TEST");
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

char**	CGIEnv::GetEnv(void) const
{
	char**		exec_env;
	size_t		size;

	size = env_.size();
	exec_env = new char*[size + 1];
	for (size_t i = 0; i < size; i++)
		exec_env[i] = const_cast<char *>(env_[i].c_str());
	exec_env[size] = NULL;

	return (exec_env);
}
