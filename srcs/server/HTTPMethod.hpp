#ifndef HTTPMETHOD_HPP
# define HTTPMETHOD_HPP

# include <string>
# include <vector>
# include <utility>
# include "HTTPRequest.hpp"
# include "Stat.hpp"

class HTTPMethod
{
	public:
		HTTPMethod();
		~HTTPMethod();

		e_StatusCode		ExecHTTPMethod(const HTTPRequest& req);

		const std::string&	GetContentType() const;
		const std::string&	GetLocation() const;
		const std::string&	GetBody() const;

		void				MethodDisplay() const;

	private:
		LocationDirective	SelectLocation(const std::vector<LocationDirective>& locations) const;
		int		Redirect(const std::string& location, const int status_code);

		// GET
		bool	GetFile(const std::string& access_path);
		bool	GetFileWithIndex(const std::string& access_path, const std::vector<std::string>& indexes);
		bool	GetAutoIndexFile(const std::string& access_path, const bool autoindex);

		// HTTPMethod
		e_StatusCode	SwitchHTTPMethod(const LocationDirective& location);
		e_StatusCode	ExecGETMethod(const Stat& st, const LocationDirective& location);
		e_StatusCode	ExecDELETEMethod(const Stat& st);
		e_StatusCode	ExecPOSTMethod(const Stat& st);

		// CGI
		bool	CheckCGIScript(const Stat& st, const LocationDirective& location);
		int		ExecCGI();

		std::string		content_type_;
		std::string		location_;
		std::string		body_;

		const HTTPRequest*			req_;
		const ServerDirective*		server_conf_;
};

#endif
