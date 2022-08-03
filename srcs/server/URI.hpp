#ifndef URI_HPP
# define URI_HPP

# include <string>

class URI
{
	public:
		URI(const std::string& root, const std::string& target);
		~URI();

		const std::string&		GetFullPath(void) const;
		const std::string&		GetPath(void) const;
		const std::string&		GetQuery(void) const;

	private:
		const std::string&		root_;
		const std::string&		target_;
		std::string				full_path_;
		std::string				path_;
		std::string				query_;
};

#endif
