#ifndef CGIENV_HPP
# define CGIENV_HPP

# include <string>
# include <vector>
# include "URI.hpp"
# include "HTTPRequest.hpp"

class CGIEnv
{
	public:
		CGIEnv(const URI& uri, const HTTPRequest& req);
		~CGIEnv(void);

		char**	GetEnv(void);

	private:
		void	SetEnv(void);
		void	AddEnv(const std::string& key, const std::string& value);

		const URI&					uri_;
		const HTTPRequest&			req_;
		const ServerDirective*		server_conf_;
		std::vector<std::string>	env_;
		char**						exec_env_;
};

#endif
