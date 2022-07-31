#ifndef REGULARFILE_HPP
# define REGULARFILE_HPP

# include <string>
# include "AIo.hpp"

class RegularFile : public AIo
{
	public:
		explicit RegularFile(const int fd);
		~RegularFile();

		std::string		GetFileContent() const;
		void			WriteToFile(const std::string& str) const;
};

#endif  // REGULARFILE_HPP
