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

class GETRESTest : public ::testing::Test
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

Config			GETRESTest::config_("conf/default.conf");
ListenSocket*	GETRESTest::lsocket_ = NULL;
ServerSocket*	GETRESTest::ssocket_ = NULL;
ClientSocket*	GETRESTest::csocket_ = NULL;

const std::map<std::string, std::string> &HTTPResponse::GetHeader() const { return headers_; }
const std::string &HTTPResponse::GetResMsg() const { return res_msg_; }
const std::vector<std::string> rm_headers = {"ETag", "Last-Modified", "Accept-Ranges", "Server", "Content-Type"};
const std::vector<std::string> cmp_headers = {"Connection", "Date", "Location"};

void HeaderCmp(std::map<std::string, std::string> model_header, std::map<std::string, std::string> res_header)
{
	std::vector<std::string>::const_iterator ite = cmp_headers.begin();
	for (; ite != cmp_headers.end(); ite++)
	{
		EXPECT_EQ(model_header[*ite], res_header[*ite]);
		// std::cout << *ite << ": " << model_header[*ite];
		// std::cout << *ite << ": " << res_header[*ite];
	}
}

void HeaderCmp(std::vector<std::string> cmp_head,
		std::map<std::string, std::string> model_header, std::map<std::string, std::string> res_header)
{
	std::vector<std::string>::const_iterator ite = cmp_head.begin();
	for (; ite != cmp_head.end(); ite++)
	{
		EXPECT_EQ(model_header[*ite], res_header[*ite]);
		// std::cout << *ite << ": " << model_header[*ite];
		// std::cout << *ite << ": " << res_header[*ite];
	}
}

TEST_F(GETRESTest, BasicTest)
{
	RunCommunication("GET /ind.html HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	Model model("localhost:8080/ind.html", rm_headers);
	EXPECT_EQ(model.GetResponse(), res.GetResMsg());
}

TEST_F(GETRESTest, NotFoundTest)
{
	RunCommunication("GET /no HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	Model model("localhost:8080/no", rm_headers);
	EXPECT_EQ(model.GetResponse(), res.GetResMsg());
}

TEST_F(GETRESTest, RootTest)
{
	RunCommunication("GET /hoge/ HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	Model model("localhost:8080/hoge/", rm_headers);
	EXPECT_EQ(model.GetResponse(), res.GetResMsg());
}

TEST_F(GETRESTest, DirRedirectTest)
{
	const std::vector<std::string> cmp_head = {"Connection", "Date"};
	RunCommunication("GET /sub1 HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	Model model("localhost:8080/sub1", rm_headers);
	HeaderCmp(cmp_head, model.GetHeader(), res.GetHeader());
	// EXPECT_EQ(model.GetResponse(), res.GetResMsg());
}

TEST_F(GETRESTest, IndexTest)
{
	RunCommunication("GET /sub1/ HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	Model model("localhost:8080/sub1/", rm_headers);
	HeaderCmp(model.GetHeader(), res.GetHeader());
	// EXPECT_EQ(model.GetResponse(), res.GetResMsg());
	// std::cout << res.GetResMsg();
	// std::cout << model.GetResponse();
}

TEST_F(GETRESTest, DirForbiddenTest)
{
	RunCommunication("GET /sub2/ HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	Model model("localhost:8080/sub2/", rm_headers);
	EXPECT_EQ(model.GetResponse(), res.GetResMsg());
}
