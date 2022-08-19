#ifndef REGULARFILE_HPP
# define REGULARFILE_HPP

# include <fcntl.h>
# include <string>
# include "AIo.hpp"
# include "Stat.hpp"

class RegularFile : public AIo
{
	public:
		RegularFile(const std::string& path, const int open_mode);
		~RegularFile();

		bool				Fail() const;
		const std::string&	GetName() const;
		size_t				GetSize() const;

		ssize_t	ReadFile(std::string* str) const;
		ssize_t	WriteToFile(const std::string& str) const;
		int		DeleteFile();

	private:
		std::string		path_;
		std::string		name_;
		size_t			size_;
		bool			failed_;
};

#endif  // REGULARFILE_HPP
