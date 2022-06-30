#ifndef SERVERDIRECTIVE_HPP
# define SERVERDIRECTIVE_HPP

# include <string>
# include <vector>
# include <map>
# include "LocationDirective.hpp"

class ServerDirective
{
	public:
		ServerDirective();
		~ServerDirective();

		void	ParseServerDirective();

	private:
		std::pair<std::string, int>			listen_;
		std::vector<std::string> 			server_names_;
		std::map<int, std::string> 			error_pages_;
		size_t								client_max_body_size_;
		std::vector<LocationDirective*>		locations_;
};

#endif  // SERVERDIRECTIVE_HPP
