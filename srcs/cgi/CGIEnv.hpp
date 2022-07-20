#ifndef CGIENV_HPP
# define CGIENV_HPP

# include <string>
# include <vector>
# include "HTTPRequest.hpp"

class CGIEnv
{
	public:
		CGIEnv(const HTTPRequest& req);
		~CGIEnv(void);

		char**	GetEnv(void) const;

	private:
		void	SetEnv(void);
		void	AddEnv(const std::string& key, const std::string& value);

		const HTTPRequest&			req_;
		std::vector<std::string>	env_;
};

#endif
