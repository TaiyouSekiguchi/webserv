#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "HTTPRequest.hpp"
#include "HTTPServer.hpp"
#include "ServerDirective.hpp"

class HTTPResponse
{
	public:
		// HTTPResponse(const int &status_code, const &HTTPRequest req, const HTTPMethod &method,
			// const ServerDirective &server_conf);
		HTTPResponse(const int &status_code, const ServerDirective &server_conf);
		~HTTPResponse();

		void SendResponse(ServerSocket *ssocket);

	private:
		static const std::pair<int, std::string> kPairs_[];
		static const std::map<int, std::string> kStatusMsg_;
		std::string res_msg_;
		std::map<std::string, std::string> headers_;

		// void SetkStatusMsg() const;
		// void AppendHeader(const HTTPRequest &req, const HTTPMethod &method);
		void AppendHeader();
		std::string CreateResponse(const int &status_code, const ServerDirective &server_conf);
		std::string HeaderFeild() const;
		std::string GetDate() const;
		std::string GenerateHTML(const int &status_code, const ServerDirective &server_conf) const;
		std::string GenerateDefaultHTML(const int &status_code) const;
		bool IsNormalStatus(const int &status_code) const;
		bool IsRedirectStatus(const int &status_code) const;
};

#endif
