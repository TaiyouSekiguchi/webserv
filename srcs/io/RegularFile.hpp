#ifndef REGULARFILE_HPP
# define REGULARFILE_HPP

# include <string>
# include <fcntl.h>
# include "AIo.hpp"
# include "Stat.hpp"

class RegularFile : public AIo
{
	public:
		RegularFile(const std::string& path, const mode_t open_mode);
		~RegularFile();

		bool				Fail() const;
		const std::string&	GetName() const;

		int		ReadFile(std::string* str) const;
		int		WriteToFile(const std::string& str) const;
		int		DeleteFile();

	private:
		std::string		path_;
		std::string		name_;
		bool			failed_;
};

#endif  // REGULARFILE_HPP
