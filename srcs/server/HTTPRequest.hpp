#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

#include <iostream>
#include <string>
#include <vector>
#include "ServerSocket.hpp"
#include "ClientClosed.hpp"

class HTTPRequest
{
	public:
		HTTPRequest();
		~HTTPRequest();

		void	ParseRequest(ServerSocket const & ssocket);
		void	RequestDisplay(void) const;

	private:
		enum	e_status
		{
			REQUEST,
			HEADER,
			BODY,
		};

		enum	e_method
		{
			GET = 0,
			POST,
			DELETE,
			NONE,
		};

		//GetLine
		std::string		save_;

		//request line
		e_method		method_;
		std::string		target_;
		std::string		version_;

		//header
		std::string		host_;

		//body
		std::string		body_;

		//func
		std::string		GetLine(ServerSocket const & ssocket);
		void			ParseRequestLine(ServerSocket const & ssocket);
		void			ParseHeaders(ServerSocket const & ssocket);

		void			RequestPart(std::string const & line);
		void			HeaderPart(std::string const & line);
		void			DoParse(std::string const & line);
};

#endif
