#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <cstdlib>
#include <cctype>
#include <cerrno>
#include "ServerSocket.hpp"
#include "ClientClosed.hpp"
#include "HTTPError.hpp"
#include "utils.hpp"

class HTTPRequest
{
	public:
		enum	e_method
		{
			GET = 0,
			POST,
			DELETE,
			NONE,
		};

		HTTPRequest();
		~HTTPRequest();

		void	ParseRequest(const ServerSocket& ssocket, const ServerDirective& server_conf);
		void	RequestDisplay(void) const;

		// Getter
		e_method						GetMethod(void) const;
		std::string						GetTarget(void) const;
		std::string						GetVersion(void) const;
		std::pair<std::string, int>		GetHost(void) const;
		std::string						GetContentLength(void) const;
		std::string						GetUserAgent(void) const;
		std::vector<std::string>		GetAcceptEncoding(void) const;
		bool							GetConnection(void) const;
		std::string						GetContentType(void) const;
		std::string						GetBody(void) const;

	private:
		typedef	void 	(HTTPRequest::*ParseFunc)(const std::string& content);

		/*
		enum	e_status
		{
			REQUEST,
			HEADER,
			BODY,
		};
		*/

		// server_conf
		size_t							client_max_body_size_;

		// GetLine
		std::string						save_;

		// request line
		e_method						method_;
		std::string						target_;
		std::string						version_;

		// header
		std::pair<std::string, int>		host_;
		std::string						content_length_;
		std::string						user_agent_;
		std::vector<std::string>		accept_encoding_;
		bool							connection_;
		std::string						content_type_;

		// body
		std::string		body_;

		// func
		void			SetServerConf(const ServerDirective& server_conf);
		std::string		GetLine(const ServerSocket& ssocket);
		void			ParseRequestLine(ServerSocket const & ssocket);
		void			ParseMethod(const std::string& method);
		void			ParseTarget(const std::string& target);
		void			ParseVersion(const std::string& version);

		void			ParseHeaders(const ServerSocket& ssocket);
		void			ParseHeader(const std::string& field, const std::string& content);
		void			ParseHost(const std::string& content);
		void			ParseContentLength(const std::string& content);
		void			ParseUserAgent(const std::string& content);
		void			ParseAcceptEncoding(const std::string& content);
		void			ParseConnection(const std::string& content);
		void			ParseContentType(const std::string& content);

		void			ParseBody(const ServerSocket& ssocket);
};

#endif
