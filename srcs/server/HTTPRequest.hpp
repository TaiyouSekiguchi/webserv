#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

#include <iostream>
#include <string>
#include <vector>
#include "ServerSocket.hpp"

class HTTPRequest
{
	public:
		HTTPRequest();
		~HTTPRequest();

		void	ParseRequest(ServerSocket & ssocket);
		void	RequestDisplay(void) const;
		bool	GetParseStatus(void) const;

	private:
		enum	e_status
		{
			REQUEST,
			HEADER,
			BODY,
			PARSE,
			OK,
			BAD,
		};

		enum	e_method
		{
			GET = 0,
			POST,
			DELETE,
		};

		e_status		request_status_;
		e_status		line_status_;

		//request line
		e_method		method_;
		std::string		target_;
		std::string		version_;

		//header
		std::string		host_;

		//body
		std::string		body_;

		void	RequestPart(std::string const & line);
		void	HeaderPart(std::string const & line);
		void	DoParse(std::string const & line);

};

#endif
