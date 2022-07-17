#include "CGIEnv.hpp"

CGIEnv::CGIEnv(void)
{
	SetEnv();
}

CGIEnv::~CGIEnv(void)
{
}

void	CGIEnv::AddEnv(const std::string& key, const std::string& value)
{
	env_.push_back(std::make_pair(key, value));
}

void	CGIEnv::SetEnv(void)
{
	AddEnv("AUTH_TYPE", "");
	AddEnv("CONTENT_LENGTH", "42");
	AddEnv("CONTENT_TYPE", "TEST");
	AddEnv("GATEWAY_INTERFACE", "CGI/1.1");
	AddEnv("HTTP_ACCEPT", "TEST");
	AddEnv("HTTP_REFERER", "TEST");
	AddEnv("HTTP_USER_AGENT", "TEST");
	AddEnv("PATH_INFO", "TEST");
	AddEnv("PATH_TRANSLATED", "TEST");
	AddEnv("QUERY_STRING", "TEST");
	AddEnv("REMOTE_ADDR", "TEST");
	AddEnv("REMOTE_HOST", "TEST");
	AddEnv("REMOTE_IDENT", "TEST");
	AddEnv("REMOTE_USER", "TEST");
	AddEnv("REQUEST_METHOD", "TEST");
	AddEnv("SCRIPT_NAME", "TEST");
	AddEnv("SERVER_NAME", "TEST");
	AddEnv("SERVER_PORT", "TEST");
	AddEnv("SERVER_PROTOCOL", "TEST");
	AddEnv("SERVER_SOFTWARE", "TEST");
}

char**	CGIEnv::GetEnv(void) const
{
	char**		exec_env;
	size_t		size;

	size = env_.size();
	exec_env = new char*[size + 1];
	for (size_t i = 0; i < size; i++)
	{
		std::string	tmp;

		tmp = env_[i].first + "=" + env_[i].second;
		exec_env[i] = new char[tmp.size() + 1];
		strlcpy(exec_env[i], tmp.c_str(), tmp.size() + 1);
	}
	exec_env[size] = NULL;

	return (exec_env);
}
