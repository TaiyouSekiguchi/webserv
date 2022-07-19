#ifndef HTTPMETHOD_HPP
# define HTTPMETHOD_HPP

# include "HTTPRequest.hpp"
# include "Stat.hpp"

class HTTPMethod
{
	public:
		HTTPMethod();
		~HTTPMethod();

		int		ExecHTTPMethod(const HTTPRequest& req, const ServerDirective& server_conf);

		const std::string&	GetContentType() const;
		const std::string&	GetLocation() const;
		const std::string&	GetBody() const;

		void				MethodDisplay() const;

	private:
		LocationDirective	SelectLocation(const std::vector<LocationDirective>& locations) const;
		int					Redirect(const std::string& location, const int status_code);

		// GET
		bool	GetFile(const std::string& access_path);
		bool	GetFileWithIndex(const std::string& access_path, const std::vector<std::string>& indexes);
		bool	GetAutoIndexFile(const std::string& access_path, const bool autoindex);

		// HTTPMethod
		int		SwitchHTTPMethod(const LocationDirective& location);
		int		ExecGETMethod(const Stat& st, const LocationDirective& location);
		int		ExecDELETEMethod(const Stat& st);
		int		ExecPOSTMethod(const Stat& st);

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
