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

		std::string		GetData(void) const;
		std::string		GetContentType(void) const;
		std::string		GetBody(void) const;

	private:
		typedef	void (CGI::*ParseFunc)(const std::string& content);

		void	ExecuteCGI(void);
		void	ParseCGI(void);
		void	SendData(const int write_pipe_fd[2], const int read_pipe_fd[2]);
		void	ReceiveData(const int write_pipe_fd[2], const int read_pipe_fd[2], const pid_t pid);
		void	ParseHeader(const std::string& line);
		void	ParseContentType(const std::string& content);

		const URI&				uri_;
		const HTTPRequest&		req_;
		const ServerDirective*	server_conf_;
		std::string				data_;
		std::string				content_type_;
		std::string				body_;
};

#endif
