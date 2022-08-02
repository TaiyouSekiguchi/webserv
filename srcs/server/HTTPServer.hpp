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
# include "AServerIoEvent.hpp"
# include "RegularFile.hpp"

class HTTPServer
{
	public:
		HTTPServer(ServerSocketEvent* ssocket_event, const ServerSocket& ssocket);
		~HTTPServer();

		bool			GetConnection() const;

		AServerIoEvent*	Run();
		AServerIoEvent*	RunCreateResponse();
		void			RunSendResponse();

		void			ExecGETMethod(const RegularFile& rfile);
		void			ExecPOSTMethod(const RegularFile& rfile);
		void			ExecDELETEMethod(const RegularFile& rfile);

	private:
		ServerSocketEvent*	ssocket_send_event_;
		const ServerSocket&	ssocket_;

		HTTPRequest*		request_;
		HTTPMethod*			method_;
		HTTPResponse*		response_;

		bool				connection_;
};

#endif  // HTTPSERVER_HPP
