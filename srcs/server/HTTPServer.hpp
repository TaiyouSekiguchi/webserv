#ifndef HTTPSERVER_HPP
# define HTTPSERVER_HPP

# include <vector>
# include "ServerSocket.hpp"
# include "Config.hpp"
# include "HTTPRequest.hpp"
# include "HTTPMethod.hpp"
# include "HTTPResponse.hpp"
# include "HTTPServerEventType.hpp"
# include "RegularFile.hpp"

class HTTPServer
{
	public:
		explicit HTTPServer(const ServerSocket& ssocket);
		~HTTPServer();

		int		GetMethodTargetFileFd() const;
		void	DeleteMethodTargetFile();

		e_HTTPServerEventType	Run();
		e_HTTPServerEventType	RunHTTPMethod(const e_HTTPServerEventType event_type);
		e_HTTPServerEventType	RunCreateResponse();
		e_HTTPServerEventType	RunSendResponse();

	private:
		const ServerSocket&	ssocket_;

		HTTPRequest*		request_;
		HTTPMethod*			method_;
		HTTPResponse*		response_;
};

#endif  // HTTPSERVER_HPP
