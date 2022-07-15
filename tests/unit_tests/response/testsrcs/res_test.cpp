#include <gtest/gtest.h>
#include <fstream>
#include "ListenSocket.hpp"
#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#include "Config.hpp"
#include "HTTPRequest.hpp"
#include "HTTPMethod.hpp"
#include "HTTPResponse.hpp"

// const std::string &HTTPResponse::GetResMsg() const { return res_msg_; }

TEST(RESTest, Normal)
{
	Config config("conf/default.conf");
	int		status_code;
	HTTPRequest		req;
	HTTPMethod		method;
	// const ServerDirective&	server_conf = ssocket->GetServerConf();
	const ServerDirective&	server_conf = config.GetServers().at(0);

	try
	{
		// req.ParseRequest(*ssocket, server_conf);
		status_code = method.ExecHTTPMethod(req, server_conf);
	}
	catch (const ClientClosed& e)
	{
		// delete ssocket;
		return;
	}
	catch (const HTTPError& e)
	{
		status_code = e.GetStatusCode();
	}
	req.RequestDisplay();
	std::cout << "status_code: " << status_code << std::endl;
	method.MethodDisplay();
	HTTPResponse	res(status_code, req, method, server_conf);
	// std::cout << res.GetResMsg() << std::endl;
}
