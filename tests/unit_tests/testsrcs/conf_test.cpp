#include <gtest/gtest.h>
#include "Config.hpp"

// テストフィクスチャ
class ConfigTest : public ::testing::Test {
	protected:
		// 最初に一回だけ実行
		static void SetUpTestCase() {}
		// 最後に一回だけ実行
		static void TearDownTestCase() {}
		// 各テストの最初に一回だけ実行
		virtual void SetUp() {}
		// 各テストの最後に一回だけ実行
		virtual void TearDown() {}
};

// テストフィクスチャを利用する場合、TEST_F(テストフィクスチャ名, テスト名)
TEST_F(ConfigTest, LexSimple)
{
	Config							config("conf/simple.conf");
	const std::vector<std::string>&	tokens = config.GetTokens();

	const char*	expected[] =
		{"server", "{", "listen", "8080", ";", "location", "/", "{", "root",
		 "html", ";", "index", "index.html", ";", "}", "}"};
	const std::vector<std::string>	expected_tokens(expected, &expected[16]);

	EXPECT_EQ(tokens.size(), (size_t)16);
	EXPECT_EQ(tokens, expected_tokens);
}

TEST_F(ConfigTest, LexComplex)
{
	Config							config("conf/complex.conf");
	const std::vector<std::string>&	tokens = config.GetTokens();

	const char*	expected[] =
		{"server", "{", "listen", "8080", ";", "location", "/", "{", "root",
		 "html", ";", "index", "\"index.html\n\"", ";", "}", "}"};
	const std::vector<std::string>	expected_tokens(expected, &expected[16]);

	EXPECT_EQ(tokens.size(), (size_t)16);
	EXPECT_EQ(tokens, expected_tokens);
}
