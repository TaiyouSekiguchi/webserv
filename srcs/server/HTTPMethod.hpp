#ifndef HTTPMETHOD_HPP
# define HTTPMETHOD_HPP

# include "HTTPRequest.hpp"

class HTTPMethod
{
	public:
		HTTPMethod();
		~HTTPMethod();

		int		ExecHTTPMethod(const HTTPRequest& req, const ServerDirective& server_conf);

	private:
		LocationDirective	SelectLocation(const std::string& target, const std::vector<LocationDirective>& locations) const;

		std::string		body_;
};

#endif
