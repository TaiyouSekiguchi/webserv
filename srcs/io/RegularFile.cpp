#include <unistd.h>
#include <sstream>
#include "RegularFile.hpp"

RegularFile::RegularFile(const std::string& path, const mode_t open_mode)
	: AIo(open(path.c_str(), open_mode)), path_(path), failed_(false)
{
	if (fd_ == -1)
	{
		failed_ = true;
		return;
	}
	std::string::size_type	slash_pos = path.find_last_of("/");
	if (slash_pos == std::string::npos)
		name_ = path_;
	else
		name_ = path_.substr(slash_pos + 1);
}

RegularFile::~RegularFile()
{
}

bool	RegularFile::Fail() const
{
	return (failed_);
}

const std::string&	RegularFile::GetName() const { return (name_); }

int		RegularFile::ReadFile(std::string* str) const
{
	if (failed_)
		throw std::runtime_error("regularfile error");

	const int			buf_size = 1024;
	char				buf[buf_size + 1];
	std::stringstream	ss;
	ssize_t				readsize;

	while ((readsize = read(fd_, &buf, buf_size)) > 0)
	{
		buf[readsize] = '\0';
		*str += buf;
	}
	return (readsize);
}

int		RegularFile::WriteToFile(const std::string& str) const
{
	if (failed_)
		throw std::runtime_error("regularfile error");

	return (write(fd_, str.c_str(), str.size()));
}

int		RegularFile::DeleteFile()
{
	if (failed_)
		throw std::runtime_error("regularfile error");

	if (close(fd_) == -1)
		return (-1);
	fd_ = -1;
	return (std::remove(path_.c_str()));
}
