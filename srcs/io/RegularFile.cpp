#include <unistd.h>
#include <sstream>
#include "RegularFile.hpp"

RegularFile::RegularFile(const int fd)
	: AIo(fd)
{
}

RegularFile::~RegularFile()
{
}

std::string	RegularFile::GetFileContent() const
{
	const int			buf_size = 1024;
	char				buf[buf_size + 1];
	std::stringstream	ss;
	ssize_t				readsize;

	while ((readsize = read(fd_, &buf, buf_size)) > 0)
	{
		buf[readsize] = '\0';
		ss << buf;
	}
	return (ss.str());
}

void	RegularFile::WriteToFile(const std::string& str) const
{
	write(fd_, str.c_str(), str.size());
}
