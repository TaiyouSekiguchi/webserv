#ifndef HTTPSERVER_HPP
# define HTTPSERVER_HPP

# include <vector>
# include "ServerSocket.hpp"
# include "EventQueue.hpp"
# include "Config.hpp"
# include "HTTPRequest.hpp"
# include "HTTPMethod.hpp"
# include "HTTPResponse.hpp"
# include "ServerSocketEvent.hpp"

class HTTPServer
{
	public:
		HTTPServer(ServerSocketEvent* ssocket_event, const ServerSocket& ssocket);
		~HTTPServer();

		bool			GetConnection() const;
		std::string		GetRequestBody() const;
		void			SetStatusCode(const e_StatusCode sc);
		void			SetResponseBody(const std::string& body);

		AIoEvent*		Run();
		AIoEvent*		RunCreateResponse();
		void			RunSendResponse();

	private:
		ServerSocketEvent*	ssocket_send_event_;
		const ServerSocket&	ssocket_;

		HTTPRequest*		request_;
		HTTPMethod*			method_;
		HTTPResponse*		response_;

		bool				connection_;
};

#endif  // HTTPSERVER_HPP
