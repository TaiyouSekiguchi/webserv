#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include <utility>
#include <map>
#include <string>
#include "HTTPRequest.hpp"
#include "HTTPMethod.hpp"
#include "ServerDirective.hpp"
#include "HTTPServerEventType.hpp"

class HTTPResponse
{
	public:
		static std::map<e_StatusCode, std::string>	kStatusMsg_;

		explicit HTTPResponse(const HTTPMethod &method);
		~HTTPResponse();

		const std::string&	GetResMsg() const;
		const bool&			GetConnection() const;

		e_HTTPServerEventType 	SendResponse(const ServerSocket& ssocket);

	private:
		static const std::pair<e_StatusCode, std::string>	kPairs_[];

		const HTTPMethod&					method_;
		std::map<std::string, std::string>	headers_;
		std::string 						res_msg_;
		bool								connection_;

		void			AppendHeaders();
		std::string		CreateResponse();
		std::string 	GetDate() const;
		std::string		HeaderField() const;
};

#endif
