#ifndef HTTPSERVER_HPP
# define HTTPSERVER_HPP

# include <vector>
# include "ServerSocket.hpp"
# include "Config.hpp"
# include "HTTPRequest.hpp"
# include "HTTPMethod.hpp"
# include "HTTPResponse.hpp"
# include "HTTPServerEvent.hpp"
# include "RegularFile.hpp"

class HTTPServer
{
	public:
		explicit HTTPServer(const ServerSocket& ssocket);
		~HTTPServer();

		const int		GetMethodTargetFileFd() const;
		void			DeleteMethodTargetFile();

		HTTPServerEvent::e_Type	Run();
		HTTPServerEvent::e_Type	RunHTTPMethod(const HTTPServerEvent::e_Type event_type);
		HTTPServerEvent::e_Type	RunCreateResponse();
		HTTPServerEvent::e_Type	RunSendResponse();

	private:
		const ServerSocket&	ssocket_;

		HTTPRequest*		request_;
		HTTPMethod*			method_;
		HTTPResponse*		response_;
};

#endif  // HTTPSERVER_HPP
