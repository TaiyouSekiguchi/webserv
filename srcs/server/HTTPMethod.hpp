#ifndef HTTPMETHOD_HPP
# define HTTPMETHOD_HPP

# include <string>
# include <vector>
# include <utility>
# include "HTTPRequest.hpp"
# include "URI.hpp"
# include "CGI.hpp"
# include "Stat.hpp"
# include "RegularFile.hpp"
# include "HTTPServerEventType.hpp"

class HTTPMethod
{
	public:
		explicit HTTPMethod(const HTTPRequest& req);
		~HTTPMethod();

		e_HTTPServerEventType	ValidateHTTPMethod();
		e_HTTPServerEventType	ValidateErrorPage(const e_StatusCode status_code);

		const std::string&	GetContentType() const;
		const std::string&	GetLocation() const;
		const std::string&	GetBody() const;
		const e_StatusCode&	GetStatusCode() const;

		int					GetTargetFileFd() const;
		void				DeleteTargetFile();

		void				ExecGETMethod();
		void				ExecPOSTMethod();
		void				ExecDELETEMethod();
		void				ReadErrorPage();

		void				MethodDisplay() const;

	private:
		LocationDirective	SelectLocation(const std::vector<LocationDirective>& locations) const;
		e_StatusCode		Redirect(const std::string& location, const e_StatusCode status_code);

		// GET
		bool	IsReadableFile(const std::string& access_path);
		bool	IsReadableFileWithIndex(const std::string& access_path, const std::vector<std::string>& indexes);
		void	SetAutoIndexContent(const std::string& access_path);

		// HTTPMethod
		e_HTTPServerEventType	ValidateAnyMethod(const LocationDirective& location);
		e_HTTPServerEventType	ValidateGETMethod(const Stat& st, const LocationDirective& location);
		e_HTTPServerEventType	ValidateDELETEMethod(const Stat& st);
		e_HTTPServerEventType	ValidatePOSTMethod(const Stat& st);

		std::string 			GenerateDefaultHTML() const;

		// CGI
		bool	CheckCGIScript(const Stat& st, const LocationDirective& location);
		e_HTTPServerEventType	ExecCGI(const URI& uri);

		const HTTPRequest&		req_;
		const ServerDirective*	server_conf_;

		std::string		content_type_;
		std::string		location_;
		std::string		body_;
		e_StatusCode	status_code_;
		RegularFile*	target_rfile_;
};

#endif
