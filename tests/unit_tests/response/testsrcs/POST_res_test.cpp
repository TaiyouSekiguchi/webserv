#include <gtest/gtest.h>
#include <fstream>
#include "ListenSocket.hpp"
#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#include "Config.hpp"
#include "HTTPRequest.hpp"
#include "HTTPMethod.hpp"
#include "HTTPResponse.hpp"
#include "Model.hpp"

class POSTRESTest : public ::testing::Test
{
	protected:
		static void SetUpTestCase()
		{
			lsocket_ = new ListenSocket(*(config_.GetServers().begin()));
			lsocket_->ListenConnection();
			csocket_ = new ClientSocket();
			csocket_->ConnectServer("127.0.0.1", 8085);
			ssocket_ = new ServerSocket(lsocket_->AcceptConnection(), lsocket_->GetServerConf());
		}
    	static void TearDownTestCase()
		{
			delete lsocket_;
			delete ssocket_;
			delete csocket_;
		}

		void	RunCommunication(const std::string& msg)
		{
			try
			{
				csocket_->SendRequest(msg);
				req_.ParseRequest(*ssocket_, ssocket_->GetServerConf());
				status_code_ = method_.ExecHTTPMethod(req_, ssocket_->GetServerConf());
			}
			catch (const HTTPError& e)
			{
				status_code_ = e.GetStatusCode();
			}
			// req_.RequestDisplay();
			// std::cout << "status_code: " << status_code_ << std::endl;
			// method_.MethodDisplay();
		}

		static Config			config_;
		static ListenSocket		*lsocket_;
		static ServerSocket 	*ssocket_;
		static ClientSocket		*csocket_;

		int						status_code_;
		HTTPRequest				req_;
		HTTPMethod				method_;
};

Config			POSTRESTest::config_("conf/post.conf");
ListenSocket*	POSTRESTest::lsocket_ = NULL;
ServerSocket*	POSTRESTest::ssocket_ = NULL;
ClientSocket*	POSTRESTest::csocket_ = NULL;

const std::vector<std::string> rm_headers = {"ETag", "Last-Modified", "Accept-Ranges", "Server", "Content-Type"};
const std::vector<std::string> cmp_headers = {"Connection", "Date", "Location"};

static void HeaderCmp(std::map<std::string, std::string> model_header, std::map<std::string, std::string> res_header)
{
	std::vector<std::string>::const_iterator ite = cmp_headers.begin();
	for (; ite != cmp_headers.end(); ite++)
	{
		EXPECT_EQ(model_header[*ite], res_header[*ite]);
		// std::cout << *ite << ": " << model_header[*ite];
		// std::cout << "res: " << *ite << ": " << res_header[*ite];
	}
}

TEST_F(POSTRESTest, NotAllowedTest)
{
	RunCommunication("POST / HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	Model model("POST", "localhost:8080/", rm_headers);
	// EXPECT_EQ(model.GetStatus(), res.GetResStatus());
	HeaderCmp(model.GetHeader(), res.GetHeader());
	// EXPECT_EQ(model.GetResponse(), res.GetResMsg());
}

TEST_F(POSTRESTest, NotFoundTest)
{
	RunCommunication("POST /upload/no HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	Model model("POST", "localhost:8080/upload/no", rm_headers);
	// EXPECT_EQ(model.GetStatus(), res.GetResStatus());
	HeaderCmp(model.GetHeader(), res.GetHeader());
	// EXPECT_EQ(model.GetResponse(), res.GetResMsg());
}

/* 
TEST_F(POSTRESTest, NotDirTest)
{
	RunCommunication("POST /upload/index.html HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	Model model("POST", "localhost:8080/upload/index.html", rm_headers);
	EXPECT_EQ(model.GetStatus(), res.GetResStatus());
	// HeaderCmp(model.GetHeader(), res.GetHeader());
	// EXPECT_EQ(model.GetResponse(), res.GetResMsg());
}

TEST_F(POSTRESTest, Upload1Test)
{
	RunCommunication("POST /upload HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	Model model("POST", "localhost:8080/upload", rm_headers);
	// EXPECT_EQ(model.GetStatus(), res.GetResStatus());
	// HeaderCmp(cmp_headers, model.GetHeader(), res.GetHeader());
	// HeaderCmp(model.GetHeader(), res.GetHeader());
	// EXPECT_EQ(model.GetResponse(), res.GetResMsg());
	std::cout << res.GetResMsg();
	std::cout << model.GetResponse();
}

TEST_F(POSTRESTest, Upload2Test)
{
	RunCommunication("POST /upload HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	EXPECT_EQ(status_code_, 201);
	EXPECT_NE(method_.GetLocation().find("/upload/16"), std::string::npos);
}
 */
