#include "CGIEnv.hpp"

CGIEnv::CGIEnv(void)
{
	SetEnv();
}

CGIEnv::~CGIEnv(void)
{
}

void	CGIEnv::SetEnv(void)
{
	env_.push_back(std::make_pair("NAME", "Taiyou"));
}

char**	CGIEnv::GetEnv(void)
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
