#ifndef URI_HPP
# define URI_HPP

# include <string>

class URI
{
	public:
		URI(const std::string& root, const std::string& target);
		~URI();

		const std::string&		GetAccessPath(void) const;
		const std::string&		GetTargetPath(void) const;
		const std::string&		GetQuery(void) const;

	private:
		std::string				access_path_;
		std::string				target_path_;
		std::string				query_;
};

#endif
