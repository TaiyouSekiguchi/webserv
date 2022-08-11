#ifndef CGI_HPP
# define CGI_HPP

# include <unistd.h>  // close
# include <sys/types.h>  // wait
# include <sys/wait.h>  // wait

# include <iostream>
# include <string>
# include <cstdlib>  // exit
# include <map>
# include <utility>

# include "CGIEnv.hpp"
# include "URI.hpp"
# include "HTTPRequest.hpp"
# include "HTTPError.hpp"
# include "HTTPStatusCode.hpp"
# include "utils.hpp"

class CGI
{
	public:
		CGI(const URI& uri, const HTTPRequest& req);
		~CGI(void);

		e_HTTPServerEventType	ExecuteCGI(void);

		std::string				GetData(void) const;
		std::string				GetContentType(void) const;
		std::string				GetLocation(void) const;
		e_StatusCode			GetStatusCode(void) const;
		std::string				GetBody(void) const;
		int						GetCgiWriteFd(void) const;
		int						GetCgiReadFd(void) const;

	private:
		typedef	void (CGI::*ParseFunc)(const std::string& content);

		void	ParseCGI(void);
		void	SendData(void);
		void	ReceiveData(const pid_t pid);
		void	ParseHeader(const std::string& line);
		void	ParseContentType(const std::string& content);
		void	ParseLocation(const std::string& content);
		void	ParseStatusCode(const std::string& content);

		const URI&				uri_;
		const HTTPRequest&		req_;
		const ServerDirective*	server_conf_;
		std::string				data_;
		std::string				content_type_;
		std::string				location_;
		e_StatusCode			status_code_;
		std::string				body_;

		pid_t					pid_;
		Pipe					write_pipe_;
		Pipe					read_pipe_;
};

#endif
