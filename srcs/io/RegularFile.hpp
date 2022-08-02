#ifndef REGULARFILE_HPP
# define REGULARFILE_HPP

# include <string>
# include "AIo.hpp"

class RegularFile : public AIo
{
	public:
		explicit RegularFile(const int fd, const std::string& path = "");
		~RegularFile();

		const std::string&	GetName() const;

		int		ReadFile(std::string* str) const;
		int		WriteToFile(const std::string& str) const;
		int		DeleteFile();

	private:
		std::string		path_;
		std::string		name_;
};

#endif  // REGULARFILE_HPP
