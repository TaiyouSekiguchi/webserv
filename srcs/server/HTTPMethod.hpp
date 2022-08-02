#ifndef HTTPMETHOD_HPP
# define HTTPMETHOD_HPP

# include <string>
# include <vector>
# include <utility>
# include "HTTPRequest.hpp"
# include "Stat.hpp"
# include "AServerIoEvent.hpp"
# include "RegularFile.hpp"

class HTTPMethod
{
	public:
		explicit HTTPMethod(const HTTPRequest& req);
		~HTTPMethod();

		AServerIoEvent*		ValidateHTTPMethod();
		AServerIoEvent*		ValidateErrorPage();

		const std::string&	GetContentType() const;
		const std::string&	GetLocation() const;
		const std::string&	GetBody() const;
		const e_StatusCode&	GetStatusCode() const;

		void				ExecGETMethod(const RegularFile& rfile);
		void				ExecPOSTMethod(const RegularFile& rfile);
		void				ExecDELETEMethod(const RegularFile& rfile);

		void				MethodDisplay() const;

	private:
		LocationDirective	SelectLocation(const std::vector<LocationDirective>& locations) const;
		e_StatusCode		Redirect(const std::string& location, const e_StatusCode status_code);

		// GET
		bool	GetFile(const std::string& access_path);
		bool	GetFileWithIndex(const std::string& access_path, const std::vector<std::string>& indexes);
		bool	GetAutoIndexFile(const std::string& access_path, const bool autoindex);

		// HTTPMethod
		AServerIoEvent*	SwitchHTTPMethod(const LocationDirective& location);
		AServerIoEvent*	ValidateGETMethod(const Stat& st, const LocationDirective& location);
		AServerIoEvent*	ValidateDELETEMethod(const Stat& st);
		AServerIoEvent*	ValidatePOSTMethod(const Stat& st);

		// CGI
		bool	CheckCGIScript(const Stat& st, const LocationDirective& location);
		int		ExecCGI();

		std::string		content_type_;
		std::string		location_;
		std::string		body_;
		e_StatusCode	status_code_;

		const HTTPRequest&			req_;
		const ServerDirective*		server_conf_;
};

#endif
