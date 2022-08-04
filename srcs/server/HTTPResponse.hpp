#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include <utility>
#include <map>
#include <string>
#include "HTTPRequest.hpp"
#include "HTTPMethod.hpp"
#include "ServerDirective.hpp"

class HTTPResponse
{
	public:
		static std::map<e_StatusCode, std::string>	kStatusMsg_;

		HTTPResponse(const HTTPRequest &req, const HTTPMethod &method);
		~HTTPResponse();

		const std::string&	GetResMsg() const;
		const bool&			GetConnection() const;

		void 				SendResponse(const ServerSocket& ssocket);

	private:
		static const std::pair<e_StatusCode, std::string>	kPairs_[];

		const HTTPRequest&					req_;
		const HTTPMethod&					method_;
		const ServerDirective*				server_conf_;
		e_StatusCode 						status_code_;
		std::map<std::string, std::string>	headers_;
		std::string 						res_msg_;
		bool 								connection_;

		void 			CheckConnection();
		void			AppendHeaders();
		void			AppendHeader(const std::string& key, const std::string& value);
		std::string		CreateResponse();
		std::string 	GetDate() const;
		bool			IsNormalStatus() const;
		std::string		HeaderField() const;
};

#endif
