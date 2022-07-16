#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "HTTPRequest.hpp"
#include "HTTPServer.hpp"
#include "ServerDirective.hpp"
#include "HTTPMethod.hpp"

class HTTPResponse
{
	public:
		HTTPResponse(int status_code, const HTTPRequest &req,
						const HTTPMethod &method, const ServerDirective &server_conf);
		~HTTPResponse();

		void SendResponse(ServerSocket *ssocket);
		const std::string &GetResMsg() const;

	private:
		static const std::pair<int, std::string> kPairs_[];
		static const std::map<int, std::string> kStatusMsg_;
		std::string res_msg_;
		std::map<std::string, std::string> headers_;
		std::string body_;

		void AppendHeader(int *status_code, const HTTPRequest &req,
			const HTTPMethod &method, const ServerDirective &server_conf);
		void ParseHeader(int *status_code, const HTTPMethod &method, const ServerDirective &server_conf);
		std::string GetDate() const;
		std::string GenerateHTML(int *status_code, const ServerDirective &server_conf);
		std::string GenerateDefaultHTML(const int &status_code) const;
		bool IsNormalStatus(const int &status_code) const;
		void SelectBody(int *status_code, const HTTPMethod &method, const ServerDirective &server_conf);
		std::string CreateResponse(const int &status_code);
		std::string HeaderFeild() const;
};

#endif
