#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "HTTPRequest.hpp"
#include "ServerDirective.hpp"
#include "HTTPMethod.hpp"

class HTTPResponse
{
	public:
		HTTPResponse(int status_code, const HTTPRequest &req, const HTTPMethod &method);
		~HTTPResponse();

		void SendResponse(const ServerSocket *ssocket);
		const std::string &GetResMsg() const;

	private:
		static const std::pair<int, std::string> kPairs_[];
		static std::map<int, std::string> kStatusMsg_;
		const HTTPRequest &req_;
		const HTTPMethod &method_;
		const ServerDirective *server_conf_;
		std::string res_msg_;
		std::map<std::string, std::string> headers_;
		std::string body_;
		int status_code_;
		std::string connection_;

		void CheckConnection();
		void AppendHeaders();
		void AppendHeader(const std::string &key, const std::string &value);
		std::string GetDate() const;
		std::string GenerateHTML();
		std::string GenerateDefaultHTML() const;
		bool IsNormalStatus() const;
		void SelectBody();
		std::string CreateResponse();
		std::string HeaderFeild() const;
};

#endif
