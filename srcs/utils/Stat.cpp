#include "Stat.hpp"
#include <sstream>

Stat::Stat(const std::string& file_path)
	: path_(file_path)
{
	failed_ = false;
	if (stat(path_.c_str(), &st_) == -1)
		failed_ = true;
}

Stat::~Stat()
{
}

const std::string	Stat::GetPath() const { return (path_); }

bool	Stat::Fail() const
{
	return (failed_);
}

bool	Stat::IsRegularFile() const
{
	if (failed_)
		throw std::runtime_error("stat error");
	return (S_ISREG(st_.st_mode));
}

bool	Stat::IsDirectory() const
{
	if (failed_)
		throw std::runtime_error("stat error");
	return (S_ISDIR(st_.st_mode));
}

const std::string	Stat::GetModifyTime() const
{
	if (failed_)
		throw std::runtime_error("stat error");

	char		buf[26];
	std::string	mtime;
	mtime = ctime_r(&(st_.st_mtimespec.tv_sec), buf);
	return (mtime.substr(0, mtime.size() - 1));
}

const std::string	Stat::GetSizeStr() const
{
	if (failed_)
		throw std::runtime_error("stat error");

	std::stringstream	ss;
	if (IsRegularFile())
		ss << st_.st_size;
	else
		ss << "-";
	return (ss.str());
}

size_t	Stat::GetSize() const
{
	if (failed_)
		throw std::runtime_error("stat error");

	return (st_.st_size);
}
