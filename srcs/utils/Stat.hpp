#ifndef STAT_HPP
# define STAT_HPP

# include <sys/stat.h>
# include <string>

class Stat
{
	public:
		explicit Stat(const std::string& file_path);
		~Stat();

		const std::string	GetPath() const;

		bool				Fail() const;
		bool				IsRegularFile() const;
		bool				IsDirectory() const;
		const std::string	GetModifyTime() const;
		const std::string	GetSize() const;

	private:
		struct stat		st_;
		std::string		path_;
		bool			failed_;
};

#endif  // STAT_HPP
