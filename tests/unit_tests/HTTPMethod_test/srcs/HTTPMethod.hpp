#ifndef HTTPMETHOD_HPP
# define HTTPMETHOD_HPP

# include <string>
# include <vector>
# include <utility>
# include "HTTPRequest.hpp"
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

		const e_StatusCode&					GetStatusCode() const;
		std::map<std::string, std::string>	GetHeaders() const;
		const std::string&					GetBody() const;

		int					GetTargetFileFd() const;
		void				DeleteTargetFile();

		void				ExecGETMethod();
		void				ExecPOSTMethod();
		void				ExecDELETEMethod();
		void				ReadErrorPage();

		void				MethodDisplay();

		const std::string&	GetLocation() { return (headers_["Location"]); }

	private:
		LocationDirective		SelectLocation(const std::vector<LocationDirective>& locations) const;
		e_HTTPServerEventType	Redirect(const std::string& return_second, const e_StatusCode status_code);
		e_HTTPServerEventType	PublishReadEvent(const e_HTTPServerEventType event_type);

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
		bool					IsConnectionCloseStatus(const e_StatusCode status_code) const;

		// CGI
		// bool	CheckCGIScript(const Stat& st, const LocationDirective& location);
		// int		ExecCGI();

		const HTTPRequest&		req_;
		const ServerDirective*	server_conf_;

		e_StatusCode						status_code_;
		std::map<std::string, std::string>	headers_;
		std::string							body_;

		RegularFile*		target_rfile_;
};

#endif
