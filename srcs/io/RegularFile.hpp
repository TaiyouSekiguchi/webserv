#ifndef REGULARFILE_HPP
# define REGULARFILE_HPP

# include <string>
# include "AIo.hpp"

class RegularFile : public AIo
{
	public:
		explicit RegularFile(const int fd);
		~RegularFile();

		int		ReadFile(std::string* str) const;
		int		WriteToFile(const std::string& str) const;
};

#endif  // REGULARFILE_HPP
