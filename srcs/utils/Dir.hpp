#ifndef DIR_HPP
# define DIR_HPP

# include <dirent.h>
# include <string>
# include <vector>

class Dir
{
	public:
		explicit Dir(const std::string& file_path);
		~Dir();

		bool							Fail() const;
		const std::vector<std::string>	GetFileNameList() const;

	private:
		DIR*			dir_;
};

#endif
