#ifndef HTTPSERVER_HPP
# define HTTPSERVER_HPP

# include <vector>
# include "ServerSocket.hpp"
# include "EventQueue.hpp"
# include "Config.hpp"
# include "HTTPRequest.hpp"
# include "HTTPMethod.hpp"
# include "HTTPResponse.hpp"

class HTTPServer
{
	public:
		HTTPServer();
		~HTTPServer();

		bool			GetConnection() const;
		std::string		GetResponseMsg() const;

		AIoEvent*		RunRequestStep(const ServerSocket& ssocket);

	private:
		HTTPRequest*	request_;
		HTTPMethod*		method_;
		HTTPResponse*	response_;

		bool			connection_;
		std::string		response_msg_;
};

#endif  // HTTPSERVER_HPP
