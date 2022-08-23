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
		int		GetToCgiPipeFd() const;
		int		GetFromCgiPipeFd() const;

		e_HTTPServerEventType	Run();
		e_HTTPServerEventType	RunExecHTTPMethod(const e_HTTPServerEventType event_type);
		e_HTTPServerEventType	RunReadErrorPage();
		e_HTTPServerEventType	RunPostToCgi();
		e_HTTPServerEventType	RunReceiveCgiResult();
		e_HTTPServerEventType	RunCreateResponse();
		e_HTTPServerEventType	RunSendResponse();

	private:
		const ServerSocket&	ssocket_;

		HTTPRequest*		request_;
		HTTPMethod*			method_;
		HTTPResponse*		response_;
};

#endif  // HTTPSERVER_HPP
