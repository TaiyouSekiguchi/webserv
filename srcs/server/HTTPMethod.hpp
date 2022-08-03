#ifndef HTTPMETHOD_HPP
# define HTTPMETHOD_HPP

# include <string>
# include <vector>
# include <utility>
# include "HTTPRequest.hpp"
# include "Stat.hpp"
# include "RegularFile.hpp"
# include "HTTPServerEvent.hpp"

class HTTPMethod
{
	public:
		explicit HTTPMethod(const HTTPRequest& req);
		~HTTPMethod();

		HTTPServerEvent::e_Type	ValidateHTTPMethod();
		HTTPServerEvent::e_Type	ValidateErrorPage(const e_StatusCode status_code);

		const std::string&	GetContentType() const;
		const std::string&	GetLocation() const;
		const std::string&	GetBody() const;
		const e_StatusCode&	GetStatusCode() const;

		const int			GetTargetFileFd() const;
		void				DeleteTargetFile();

		void				ExecGETMethod();
		void				ExecPOSTMethod();
		void				ExecDELETEMethod();

		void				MethodDisplay() const;

	private:
		LocationDirective	SelectLocation(const std::vector<LocationDirective>& locations) const;
		e_StatusCode		Redirect(const std::string& location, const e_StatusCode status_code);

		// GET
		bool	GetFile(const std::string& access_path);
		bool	GetFileWithIndex(const std::string& access_path, const std::vector<std::string>& indexes);
		bool	GetAutoIndexFile(const std::string& access_path, const bool autoindex);

		// HTTPMethod
		HTTPServerEvent::e_Type	ValidateAnyMethod(const LocationDirective& location);
		HTTPServerEvent::e_Type	ValidateGETMethod(const Stat& st, const LocationDirective& location);
		HTTPServerEvent::e_Type	ValidateDELETEMethod(const Stat& st);
		HTTPServerEvent::e_Type	ValidatePOSTMethod(const Stat& st);

		// CGI
		bool	CheckCGIScript(const Stat& st, const LocationDirective& location);
		int		ExecCGI();

		const HTTPRequest&			req_;
		const ServerDirective*		server_conf_;

		std::string		content_type_;
		std::string		location_;
		std::string		body_;
		e_StatusCode	status_code_;
		RegularFile*	target_rfile_;
};

#endif
