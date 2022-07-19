#include <cstddef>
#include <exception>
#include "Dir.hpp"

Dir::Dir(const std::string& file_path)
{
	failed_ = false;
	dir_ = opendir(file_path.c_str());
	if (dir_ == NULL)
		failed_ = true;
}

Dir::~Dir()
{
	closedir(dir_);
}

bool	Dir::Fail() const
{
	return (failed_);
}

const std::string	Dir::GetValidFileName()
{
	if (failed_)
		throw std::runtime_error("dir error");

	std::string		name;

	while ((dirent_ = readdir(dir_)) != NULL)
	{
		name = dirent_->d_name;
		if (name == "." || name == "..")
			continue;
		else if (dirent_->d_type != DT_REG && dirent_->d_type != DT_DIR)
			continue;

		if (dirent_->d_type == DT_DIR)
			name += "/";
		return (name);
	}
	return ("");
}
