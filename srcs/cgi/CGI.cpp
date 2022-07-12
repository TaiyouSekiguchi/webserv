#include "CGI.hpp"

CGI::CGI(void)
{
}

CGI::~CGI(void)
{
}


void	CGI::ExecuteCGI(const std::string& file_path)
{
	(void)file_path;

}

void	CGI::ParseCGI(void)
{


}

std::string		CGI::GetContentType(void) const { return (content_type_); }
std::string		CGI::GetBody(void) const { return (body_); }
