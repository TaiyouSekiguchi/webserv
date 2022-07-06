#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cerrno>
#include <unistd.h>
#include "ServerSocket.hpp"
#include "ClientClosed.hpp"
#include "HTTPError.hpp"

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

		void	ParseRequest(ServerSocket const & ssocket);
		void	RequestDisplay(void) const;

		//Getter
		e_method		GetMethod(void) const;
		std::string		GetTarget(void) const;
		std::string		GetVersion(void) const;
		std::string		GetHost(void) const;
		size_t			GetContentLength(void) const;
		std::string		GetBody(void) const;

	private:
		enum	e_status
		{
			REQUEST,
			HEADER,
			BODY,
		};

		//GetLine
		std::string		save_;

		//request line
		e_method		method_;
		std::string		target_;
		std::string		version_;

		//header
		std::string		host_;
		size_t			content_length_;

		//body
		std::string		body_;

		//func
		std::string		GetLine(ServerSocket const & ssocket);
		void			ParseRequestLine(ServerSocket const & ssocket);
		void			ParseMethod(std::string const & method);
		void			ParseTarget(std::string const & target);
		void			ParseVersion(std::string const & version);

		void			ParseHeaders(ServerSocket const & ssocket);
		void			ParseHeader(std::vector<std::string> const & list);
		void			ParseHost(std::vector<std::string> const & list);
		void			ParseContentLength(std::vector<std::string> const & list);

		void			ParseBody(ServerSocket const & ssocket);
};

#endif
