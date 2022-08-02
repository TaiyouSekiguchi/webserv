#include <unistd.h>
#include <sstream>
#include "RegularFile.hpp"

RegularFile::RegularFile(const int fd, const std::string& path)
	: AIo(fd), path_(path)
{
	std::string::size_type	slash_pos = path.find_last_of("/");
	if (slash_pos == std::string::npos)
		name_ = path_;
	else
		name_ = path_.substr(slash_pos + 1);
}

RegularFile::~RegularFile()
{
}

const std::string&	RegularFile::GetName() const { return (name_); }

int		RegularFile::ReadFile(std::string* str) const
{
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
	return (write(fd_, str.c_str(), str.size()));
}

int		RegularFile::DeleteFile()
{
	if (close(fd_) == -1)
		return (-1);
	fd_ = -1;
	return (std::remove(path_.c_str()));
}
