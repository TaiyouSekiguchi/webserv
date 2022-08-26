#ifndef REGULARFILE_HPP
# define REGULARFILE_HPP

# include <fcntl.h>
# include <string>
# include "AIo.hpp"
# include "Stat.hpp"

class RegularFile : public AIo
{
	public:
		RegularFile(const std::string& path, const int open_mode, const int create_mode = 0);
		~RegularFile();

		const std::string&	GetName() const;
		size_t				GetSize() const;

		ssize_t	ReadFile(std::string* str) const;
		ssize_t	WriteToFile(const std::string& str) const;
		int		DeleteFile();

	private:
		std::string		path_;
		std::string		name_;
		size_t			size_;
};

#endif  // REGULARFILE_HPP
