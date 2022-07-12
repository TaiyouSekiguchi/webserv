#include <gtest/gtest.h>
#include "HTTPServer.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"

TEST(Response, CreateResponse)
{
	HTTPRequest req(HTTPRequest::GET);
	HTTPResponse res(200, req);
	std::cout << res.GetResMsg() << std::endl;
}
