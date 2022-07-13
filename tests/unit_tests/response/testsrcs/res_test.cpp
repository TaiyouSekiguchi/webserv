#include <gtest/gtest.h>
#include "HTTPServer.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"

// explicit HTTPRequest(e_method method);

HTTPRequest::HTTPRequest(HTTPRequest::e_method method)
{
	method_ = method;
}

TEST(Response, CreateResponse)
{
	Config config("../../../conf/default.conf");
	std::vector<ServerDirective> servers = config.GetServers();
	HTTPRequest req(HTTPRequest::GET);
	HTTPResponse res(404, req, servers.at(0));
}
