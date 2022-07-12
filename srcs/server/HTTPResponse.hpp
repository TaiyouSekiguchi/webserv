#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "HTTPRequest.hpp"
#include "ServerDirective.hpp"
#include "HTTPServer.hpp"

class HTTPResponse
{
	public:
		HTTPResponse(int status_code, HTTPRequest req, HTTPMethod method, ServerDirective server_conf);
		~HTTPResponse();

		std::string GetResMsg() const;
		void SendResponse(ServerSocket *ssocket);

	private:
		const std::map<int, std::string> status_msg_;
		std::string res_msg_;

		std::string CreateResponse(int status_code, HTTPRequest req);
		std::string AppendHeader(HTTPRequest req);
		std::string GetDate() const;
		std::string FindMsg(int status_code) const;
};

#endif
