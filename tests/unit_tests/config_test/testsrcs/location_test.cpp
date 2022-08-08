#include <gtest/gtest.h>
#include "Config.hpp"

// location
TEST(LocationTest, Valid)
{
	Config	config("conf/location/location/valid.conf");
	std::vector<ServerDirective>::const_iterator	sitr = config.GetServers().begin();
	std::vector<LocationDirective>::const_iterator	litr = sitr->GetLocations().begin();

	EXPECT_EQ(litr->GetPath(), "/");
	EXPECT_EQ(++litr, sitr->GetLocations().end());
	litr = (++sitr)->GetLocations().begin();
	EXPECT_EQ(litr->GetPath(), "/");
	EXPECT_EQ((++litr)->GetPath(), "a");
	EXPECT_EQ((++litr)->GetPath(), "/a");
	EXPECT_EQ(++litr, sitr->GetLocations().end());
}
TEST(LocationTest, Error)
{
	EXPECT_ANY_THROW({Config config("conf/location/location/err_braces.conf");});
	EXPECT_ANY_THROW({Config config("conf/location/location/err_special.conf");});
	EXPECT_ANY_THROW({Config config("conf/location/location/err_duplicate.conf");});
}

// root
TEST(RootTest, Valid)
{
	Config	config("conf/location/root/valid.conf");
	std::vector<ServerDirective>::const_iterator	sitr = config.GetServers().begin();
	std::vector<LocationDirective>::const_iterator	litr = sitr->GetLocations().begin();

	EXPECT_EQ(litr->GetRoot(), "html");
	EXPECT_EQ((++litr)->GetRoot(), "html/");
	EXPECT_EQ((++litr)->GetRoot(), "1");
}
TEST(RootTest, Error)
{
	EXPECT_ANY_THROW({Config config("conf/location/root/err_empty.conf");});
	EXPECT_ANY_THROW({Config config("conf/location/root/err_special.conf");});
	EXPECT_ANY_THROW({Config config("conf/location/root/err_duplicate.conf");});
}

// index
TEST(IndexTest, Valid)
{
	Config	config("conf/location/index/valid.conf");
	std::vector<ServerDirective>::const_iterator	sitr = config.GetServers().begin();
	std::vector<LocationDirective>::const_iterator	litr = sitr->GetLocations().begin();

	const std::string	s1[2] = {"index.html", "index.htm"};
	const std::string	s2[1] = {"a"};
	std::vector<std::string> expected;
	expected.assign(s1, s1 + 1);	EXPECT_EQ(litr->GetIndex(), expected);
	expected.assign(s1, s1 + 2);	EXPECT_EQ((++litr)->GetIndex(), expected);
	expected.assign(s2, s2 + 1);	EXPECT_EQ((++litr)->GetIndex(), expected);
}
TEST(IndexTest, Error)
{
	EXPECT_ANY_THROW({Config config("conf/location/index/err_empty.conf");});
	EXPECT_ANY_THROW({Config config("conf/location/index/err_special.conf");});
}

// return
TEST(ReturnTest, Valid)
{
	Config	config("conf/location/return/valid.conf");
	std::vector<ServerDirective>::const_iterator	sitr = config.GetServers().begin();
	std::vector<LocationDirective>::const_iterator	litr = sitr->GetLocations().begin();

	EXPECT_EQ(litr->GetReturn(), std::make_pair(static_cast<e_StatusCode>(301), std::string("http://localhost:8080")));
	EXPECT_EQ((++litr)->GetReturn(), std::make_pair(static_cast<e_StatusCode>(301), std::string("")));
	EXPECT_EQ((++litr)->GetReturn(), std::make_pair(static_cast<e_StatusCode>(302), std::string("http://localhost:8080")));
	EXPECT_EQ((++litr)->GetReturn(), std::make_pair(static_cast<e_StatusCode>(1), std::string("abc")));
	EXPECT_EQ((++litr)->GetReturn(), std::make_pair(static_cast<e_StatusCode>(999), std::string("1")));
}
TEST(ReturnTest, Error)
{
	EXPECT_ANY_THROW({Config config("conf/location/return/err_empty.conf");});
	EXPECT_ANY_THROW({Config config("conf/location/return/err_over.conf");});
	EXPECT_ANY_THROW({Config config("conf/location/return/err_char.conf");});
}

// autoindex
TEST(AutoIndexTest, Valid)
{
	Config	config("conf/location/autoindex/valid.conf");
	std::vector<ServerDirective>::const_iterator	sitr = config.GetServers().begin();
	std::vector<LocationDirective>::const_iterator	litr = sitr->GetLocations().begin();

	EXPECT_EQ(litr->GetAutoIndex(), false);
	EXPECT_EQ((++litr)->GetAutoIndex(), true);
	EXPECT_EQ((++litr)->GetAutoIndex(), false);
}
TEST(AutoIndexTest, Error)
{
	EXPECT_ANY_THROW({Config config("conf/location/autoindex/err_empty.conf");});
	EXPECT_ANY_THROW({Config config("conf/location/autoindex/err_num.conf");});
	EXPECT_ANY_THROW({Config config("conf/location/autoindex/err_arg_num.conf");});
	EXPECT_ANY_THROW({Config config("conf/location/autoindex/err_duplicate.conf");});
}

// allowed_methods
TEST(AllowedMethodsTest, Valid)
{
	Config	config("conf/location/allowed_methods/valid.conf");
	std::vector<ServerDirective>::const_iterator	sitr = config.GetServers().begin();
	std::vector<LocationDirective>::const_iterator	litr = sitr->GetLocations().begin();

	const std::string	s[3] = {"GET", "POST", "DELETE"};
	std::vector<std::string> expected;
	expected.assign(s, s + 1);	EXPECT_EQ(litr->GetAllowedMethods(), expected);
	expected.assign(s, s + 1);	EXPECT_EQ((++litr)->GetAllowedMethods(), expected);
	expected.assign(s, s + 2);	EXPECT_EQ((++litr)->GetAllowedMethods(), expected);
	expected.assign(s, s + 3);	EXPECT_EQ((++litr)->GetAllowedMethods(), expected);
}
TEST(AllowedMethodsTest, Error)
{
	EXPECT_ANY_THROW({Config config("conf/location/allowed_methods/err_empty.conf");});
	EXPECT_ANY_THROW({Config config("conf/location/allowed_methods/err_unknown.conf");});
}

// upload_root
TEST(UploadRootTest, Valid)
{
	Config	config("conf/location/upload_root/valid.conf");
	std::vector<ServerDirective>::const_iterator	sitr = config.GetServers().begin();
	std::vector<LocationDirective>::const_iterator	litr = sitr->GetLocations().begin();

	EXPECT_EQ(litr->GetUploadRoot(), "html");
	EXPECT_EQ((++litr)->GetUploadRoot(), "html/");
	EXPECT_EQ((++litr)->GetUploadRoot(), "1");
}
TEST(UploadRootTest, Error)
{
	EXPECT_ANY_THROW({Config config("conf/location/upload_root/err_empty.conf");});
	EXPECT_ANY_THROW({Config config("conf/location/upload_root/err_special.conf");});
	EXPECT_ANY_THROW({Config config("conf/location/upload_root/err_duplicate.conf");});
}

// cgi_enable_extension
TEST(CGIEnableExtensionTest, Valid)
{
	Config	config("conf/location/cgi_enable_extension/valid.conf");
	std::vector<ServerDirective>::const_iterator	sitr = config.GetServers().begin();
	std::vector<LocationDirective>::const_iterator	litr = sitr->GetLocations().begin();

	const std::string	s[1] = {"pl"};
	std::vector<std::string> expected;
	expected.assign(s, s + 1);	EXPECT_EQ(litr->GetCGIEnableExtension(), expected);
	expected.assign(s, s + 1);	EXPECT_EQ((++litr)->GetCGIEnableExtension(), expected);
}
TEST(CGIEnableExtensionTest, Error)
{
	EXPECT_ANY_THROW({Config config("conf/location/cgi_enable_extension/err_empty.conf");});
	EXPECT_ANY_THROW({Config config("conf/location/cgi_enable_extension/err_unknown.conf");});
}
