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
		HTTPResponse(const int &status_code, const HTTPRequest &req, const ServerDirective &server_conf);
		~HTTPResponse();

		void SendResponse(ServerSocket *ssocket);

	private:
		static const size_t BUF_SIZE;
		static const std::map<int, std::string> STATUS_MSG_;
		std::string res_msg_;
		std::map<std::string, std::string> headers_;

		// void AppendHeader(const HTTPRequest &req, const HTTPMethod &method);
		void AppendHeader(const HTTPRequest &req);
		std::string CreateResponse(const int &status_code, const ServerDirective &server_conf);
		std::string HeaderFeild() const;
		std::string GetDate() const;
		std::string GenerateHTML(const int &status_code, const ServerDirective &server_conf) const;
		bool IsErrorStatus(const int &status_code) const;
};

#endif
