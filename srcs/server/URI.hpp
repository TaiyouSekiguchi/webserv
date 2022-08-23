#ifndef URI_HPP
# define URI_HPP

# include <string>
# include "LocationDirective.hpp"

class URI
{
	public:
		URI(const LocationDirective& location, const std::string& target);
		~URI();

		const std::string&		GetAccessPath(void) const;
		const std::string&		GetUploadAccessPath(void) const;
		const std::string&		GetTargetPath(void) const;
		const std::string&		GetQuery(void) const;

	private:
		std::string				access_path_;
		std::string				upload_access_path_;
		std::string				target_path_;
		std::string				query_;
};

#endif
