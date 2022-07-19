#ifndef DIR_HPP
# define DIR_HPP

# include <dirent.h>
# include <string>

class Dir
{
	public:
		explicit Dir(const std::string& file_path);
		~Dir();

		bool				Fail() const;
		const std::string	GetValidFileName();

	private:
		DIR*			dir_;
		struct dirent*	dirent_;
};

#endif
