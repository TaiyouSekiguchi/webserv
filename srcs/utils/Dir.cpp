#include <cstddef>
#include <exception>
#include "Dir.hpp"

Dir::Dir(const std::string& file_path)
{
	dir_ = opendir(file_path.c_str());
}

Dir::~Dir()
{
	if (dir_)
		closedir(dir_);
}

bool	Dir::Fail() const
{
	return (dir_ == NULL);
}

const std::vector<std::string>	Dir::GetFileNameList() const
{
	if (dir_ == NULL)
		throw std::runtime_error("dir error");

	std::vector<std::string>	names;
	std::string					name;
	struct dirent*				dirent;

	while ((dirent = readdir(dir_)) != NULL)
	{
		name = dirent->d_name;
		if (name == "." || name == "..")
			continue;
		else if (dirent->d_type != DT_REG && dirent->d_type != DT_DIR)
			continue;
		if (dirent->d_type == DT_DIR)
			name += "/";
		names.push_back(name);
	}
	return (names);
}
